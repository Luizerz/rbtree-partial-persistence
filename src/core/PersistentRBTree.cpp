#include "PersistentRBTree.hpp"
#include "query/TreeFormatter.hpp"
#include "query/SuccessorFinder.hpp"
#include "io/OutputWriter.hpp"
#include <sstream>
#include <climits>

PersistentRBTree::PersistentRBTree()
    : engine_(store_), curVer_(0) {
    for (int i = 0; i < MAX_VERSIONS; ++i)
        roots_[i] = store_.NIL;
}

void PersistentRBTree::insert(int value) {
    if (curVer_ + 1 >= MAX_VERSIONS) return;
    ++curVer_;
    roots_[curVer_] = roots_[curVer_ - 1];

    Node* z = store_.makeNode(value);
    Node* y = store_.NIL;
    Node* x = roots_[curVer_];

    while (x != store_.NIL) {
        y = x;
        x = (z->key < x->key) ? store_.getLeft(x, curVer_) : store_.getRight(x, curVer_);
    }

    store_.setParent(z, y, curVer_, roots_);
    if      (y == store_.NIL)     roots_[curVer_] = z;
    else if (z->key < y->key)     store_.setLeft(y,  z, curVer_, roots_);
    else                           store_.setRight(y, z, curVer_, roots_);

    engine_.insertFixUp(z, roots_, curVer_);
}

void PersistentRBTree::remove(int value) {
    if (curVer_ + 1 >= MAX_VERSIONS) return;
    ++curVer_;
    roots_[curVer_] = roots_[curVer_ - 1];

    Node* z = engine_.searchVer(roots_[curVer_], value, curVer_);
    if (z == store_.NIL) return;

    Node* y     = z;
    Color yOrig = store_.getColor(y, curVer_);
    Node* x     = store_.NIL;
    Node* xpar  = store_.NIL;

    if (store_.getLeft(z, curVer_) == store_.NIL) {
        x    = store_.getRight(z, curVer_);
        xpar = store_.getParent(z, curVer_);
        engine_.transplant(z, x, roots_, curVer_);
    } else if (store_.getRight(z, curVer_) == store_.NIL) {
        x    = store_.getLeft(z, curVer_);
        xpar = store_.getParent(z, curVer_);
        engine_.transplant(z, x, roots_, curVer_);
    } else {
        y     = engine_.minVer(store_.getRight(z, curVer_), curVer_);
        yOrig = store_.getColor(y, curVer_);
        x     = store_.getRight(y, curVer_);

        if (store_.getParent(y, curVer_) == z) {
            xpar = y;
        } else {
            xpar = store_.getParent(y, curVer_);
            engine_.transplant(y, x, roots_, curVer_);
            store_.setRight(y, store_.getRight(z, curVer_), curVer_, roots_);
            store_.setParent(store_.getRight(y, curVer_), y, curVer_, roots_);
        }
        engine_.transplant(z, y, roots_, curVer_);
        store_.setLeft(y,  store_.getLeft(z, curVer_), curVer_, roots_);
        store_.setParent(store_.getLeft(y, curVer_), y, curVer_, roots_);
        store_.setColor(y, store_.getColor(z, curVer_), curVer_, roots_);
    }

    if (yOrig == BLACK)
        engine_.deleteFixUp(x, xpar, roots_, curVer_);
}

void PersistentRBTree::printVersion(int v, std::ostream& out) {
    int tv = clamp(v);
    std::ostringstream buf;
    buf << "IMP " << v << "\n";
    TreeQuery::inorder(roots_[tv], 0, tv, store_, buf);
    buf << ";";
    emit(buf.str(), out);
}

void PersistentRBTree::successor(int x, int v, std::ostream& out) {
    int tv   = clamp(v);
    int best = TreeQuery::successor(roots_[tv], x, tv, store_);
    std::ostringstream buf;
    buf << "SUC " << x << " " << v << " ";
    if (best == INT_MAX) buf << "infinito";
    else                 buf << best;
    emit(buf.str(), out);
}

int PersistentRBTree::currentVersion() const { return curVer_; }

int PersistentRBTree::clamp(int v) const {
    if (v < 0) return 0;
    return (v > curVer_) ? curVer_ : v;
}
