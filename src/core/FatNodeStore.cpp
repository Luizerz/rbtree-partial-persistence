#include "FatNodeStore.hpp"
#include <algorithm>

FatNodeStore::FatNodeStore() {
    NIL              = new Node();
    NIL->key         = 0;
    NIL->origLeft    = NIL;
    NIL->origRight   = NIL;
    NIL->origParent  = NIL;
    NIL->origColor   = BLACK;
}

FatNodeStore::~FatNodeStore() {
    for (auto n : allNodes_) delete n;
    delete NIL;
}

Node* FatNodeStore::makeNode(int key) {
    Node* n       = new Node();
    n->key        = key;
    n->origLeft   = NIL;
    n->origRight  = NIL;
    n->origParent = NIL;
    n->origColor  = RED;
    allNodes_.push_back(n);
    return n;
}

Node* FatNodeStore::getLeft(Node* n, int v) const {
    if (n == NIL) return NIL;
    return lastLE<Node*>(n->mods, v, Field::LEFT, n->origLeft);
}

Node* FatNodeStore::getRight(Node* n, int v) const {
    if (n == NIL) return NIL;
    return lastLE<Node*>(n->mods, v, Field::RIGHT, n->origRight);
}

Node* FatNodeStore::getParent(Node* n, int v) const {
    if (n == NIL) return NIL;
    return lastLE<Node*>(n->mods, v, Field::PARENT, n->origParent);
}

Color FatNodeStore::getColor(Node* n, int v) const {
    if (n == NIL) return BLACK;
    return lastLE<Color>(n->mods, v, Field::COLOR, n->origColor);
}

int FatNodeStore::totalMods(Node* n) const {
    return (int)n->mods.size();
}

void FatNodeStore::removeBackPtr(Node* from, Node* n) {
    auto& bp = from->backPointers;
    bp.erase(std::remove(bp.begin(), bp.end(), n), bp.end());
}

Node* FatNodeStore::handleOverflow(Node* old, int v, Node* roots[]) {
    Node* n       = new Node();
    n->key        = old->key;
    n->origLeft   = getLeft(old,   v);
    n->origRight  = getRight(old,  v);
    n->origParent = getParent(old, v);
    n->origColor  = getColor(old,  v);
    allNodes_.push_back(n);

    // Passo 1: substituir old por n' nos backPointers de quem old apontava.
    if (n->origLeft   != NIL) { removeBackPtr(n->origLeft,   old); n->origLeft->backPointers.push_back(n); }
    if (n->origRight  != NIL) { removeBackPtr(n->origRight,  old); n->origRight->backPointers.push_back(n); }
    if (n->origParent != NIL) { removeBackPtr(n->origParent, old); n->origParent->backPointers.push_back(n); }

    // Passo 2: redirecionar para n' todos os nós que apontavam para old.
    std::vector<Node*> bpCopy = old->backPointers;
    for (Node* m : bpCopy) {
        if      (getLeft(m,   v) == old) setLeft(m,   n, v, roots);
        else if (getRight(m,  v) == old) setRight(m,  n, v, roots);
        else if (getParent(m, v) == old) setParent(m, n, v, roots);
    }

    if (n->origParent == NIL) roots[v] = n;
    return n;
}

Node* FatNodeStore::setLeft(Node* n, Node* val, int v, Node* roots[]) {
    if (n == NIL) return NIL;
    Node* oldVal = lastLE<Node*>(n->mods, v, Field::LEFT, n->origLeft);
    if (oldVal != NIL) removeBackPtr(oldVal, n);
    if (val    != NIL) val->backPointers.push_back(n);
    n->mods.push_back({v, Field::LEFT, val});
    return (totalMods(n) > D) ? handleOverflow(n, v, roots) : n;
}

Node* FatNodeStore::setRight(Node* n, Node* val, int v, Node* roots[]) {
    if (n == NIL) return NIL;
    Node* oldVal = lastLE<Node*>(n->mods, v, Field::RIGHT, n->origRight);
    if (oldVal != NIL) removeBackPtr(oldVal, n);
    if (val    != NIL) val->backPointers.push_back(n);
    n->mods.push_back({v, Field::RIGHT, val});
    return (totalMods(n) > D) ? handleOverflow(n, v, roots) : n;
}

Node* FatNodeStore::setParent(Node* n, Node* val, int v, Node* roots[]) {
    if (n == NIL) return NIL;
    Node* oldVal = lastLE<Node*>(n->mods, v, Field::PARENT, n->origParent);
    if (oldVal != NIL) removeBackPtr(oldVal, n);
    if (val    != NIL) val->backPointers.push_back(n);
    n->mods.push_back({v, Field::PARENT, val});
    return (totalMods(n) > D) ? handleOverflow(n, v, roots) : n;
}

Node* FatNodeStore::setColor(Node* n, Color val, int v, Node* roots[]) {
    if (n == NIL) return NIL;
    n->mods.push_back({v, Field::COLOR, val});
    return (totalMods(n) > D) ? handleOverflow(n, v, roots) : n;
}
