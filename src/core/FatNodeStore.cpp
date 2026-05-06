#include "FatNodeStore.hpp"

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
    return lastLE(n->leftMods, v, n->origLeft);
}

Node* FatNodeStore::getRight(Node* n, int v) const {
    if (n == NIL) return NIL;
    return lastLE(n->rightMods, v, n->origRight);
}

Node* FatNodeStore::getParent(Node* n, int v) const {
    if (n == NIL) return NIL;
    return lastLE(n->parentMods, v, n->origParent);
}

Color FatNodeStore::getColor(Node* n, int v) const {
    if (n == NIL) return BLACK;
    return lastLE(n->colorMods, v, n->origColor);
}

int FatNodeStore::totalMods(Node* n) const {
    return (int)(n->leftMods.size() + n->rightMods.size() +
                 n->parentMods.size() + n->colorMods.size());
}

Node* FatNodeStore::handleOverflow(Node* old, int v, Node* roots[]) {
    Node* n       = new Node();
    n->key        = old->key;
    n->origLeft   = getLeft(old,   v);
    n->origRight  = getRight(old,  v);
    n->origParent = getParent(old, v);
    n->origColor  = getColor(old,  v);
    allNodes_.push_back(n);

    Node* par = n->origParent;
    if      (par == NIL)                  roots[v] = n;
    else if (getLeft(par, v) == old)       setLeft(par,  n, v, roots);
    else                                    setRight(par, n, v, roots);

    if (n->origLeft  != NIL) setParent(n->origLeft,  n, v, roots);
    if (n->origRight != NIL) setParent(n->origRight, n, v, roots);
    return n;
}

Node* FatNodeStore::setLeft(Node* n, Node* val, int v, Node* roots[]) {
    if (n == NIL) return NIL;
    n->leftMods.push_back({v, val});
    return (totalMods(n) > D) ? handleOverflow(n, v, roots) : n;
}

Node* FatNodeStore::setRight(Node* n, Node* val, int v, Node* roots[]) {
    if (n == NIL) return NIL;
    n->rightMods.push_back({v, val});
    return (totalMods(n) > D) ? handleOverflow(n, v, roots) : n;
}

Node* FatNodeStore::setParent(Node* n, Node* val, int v, Node* roots[]) {
    if (n == NIL) return NIL;
    n->parentMods.push_back({v, val});
    return (totalMods(n) > D) ? handleOverflow(n, v, roots) : n;
}

Node* FatNodeStore::setColor(Node* n, Color val, int v, Node* roots[]) {
    if (n == NIL) return NIL;
    n->colorMods.push_back({v, val});
    return (totalMods(n) > D) ? handleOverflow(n, v, roots) : n;
}
