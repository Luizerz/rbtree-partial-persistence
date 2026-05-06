#include "RBTreeEngine.hpp"

RBTreeEngine::RBTreeEngine(FatNodeStore& store) : store_(store) {}

void RBTreeEngine::rotateLeft(Node* x, Node* roots[], int v) {
    Node* y    = store_.getRight(x, v);
    Node* yL   = store_.getLeft(y, v);
    Node* xPar = store_.getParent(x, v);

    store_.setRight(x, yL, v, roots);
    if (yL != store_.NIL) store_.setParent(yL, x, v, roots);

    store_.setParent(y, xPar, v, roots);
    if      (xPar == store_.NIL)                       roots[v] = y;
    else if (x == store_.getLeft(xPar, v))  store_.setLeft(xPar,  y, v, roots);
    else                                    store_.setRight(xPar, y, v, roots);

    store_.setLeft(y, x, v, roots);
    store_.setParent(x, y, v, roots);
}

void RBTreeEngine::rotateRight(Node* x, Node* roots[], int v) {
    Node* y    = store_.getLeft(x, v);
    Node* yR   = store_.getRight(y, v);
    Node* xPar = store_.getParent(x, v);

    store_.setLeft(x, yR, v, roots);
    if (yR != store_.NIL) store_.setParent(yR, x, v, roots);

    store_.setParent(y, xPar, v, roots);
    if      (xPar == store_.NIL)                        roots[v] = y;
    else if (x == store_.getRight(xPar, v)) store_.setRight(xPar, y, v, roots);
    else                                    store_.setLeft(xPar,  y, v, roots);

    store_.setRight(y, x, v, roots);
    store_.setParent(x, y, v, roots);
}

void RBTreeEngine::insertFixUp(Node* z, Node* roots[], int v) {
    while (store_.getColor(store_.getParent(z, v), v) == RED) {
        Node* par  = store_.getParent(z, v);
        Node* gpar = store_.getParent(par, v);

        if (par == store_.getLeft(gpar, v)) {
            Node* uncle = store_.getRight(gpar, v);
            if (store_.getColor(uncle, v) == RED) {
                store_.setColor(par,   BLACK, v, roots);
                store_.setColor(uncle, BLACK, v, roots);
                store_.setColor(gpar,  RED,   v, roots);
                z = gpar;
            } else {
                if (z == store_.getRight(par, v)) {
                    z = par;
                    rotateLeft(z, roots, v);
                    par  = store_.getParent(z, v);
                    gpar = store_.getParent(par, v);
                }
                store_.setColor(par,  BLACK, v, roots);
                store_.setColor(gpar, RED,   v, roots);
                rotateRight(gpar, roots, v);
            }
        } else {
            Node* uncle = store_.getLeft(gpar, v);
            if (store_.getColor(uncle, v) == RED) {
                store_.setColor(par,   BLACK, v, roots);
                store_.setColor(uncle, BLACK, v, roots);
                store_.setColor(gpar,  RED,   v, roots);
                z = gpar;
            } else {
                if (z == store_.getLeft(par, v)) {
                    z = par;
                    rotateRight(z, roots, v);
                    par  = store_.getParent(z, v);
                    gpar = store_.getParent(par, v);
                }
                store_.setColor(par,  BLACK, v, roots);
                store_.setColor(gpar, RED,   v, roots);
                rotateLeft(gpar, roots, v);
            }
        }
    }
    store_.setColor(roots[v], BLACK, v, roots);
}

void RBTreeEngine::transplant(Node* u, Node* rep, Node* roots[], int v) {
    Node* uPar = store_.getParent(u, v);
    if      (uPar == store_.NIL)                       roots[v] = rep;
    else if (u == store_.getLeft(uPar, v))  store_.setLeft(uPar,  rep, v, roots);
    else                                    store_.setRight(uPar, rep, v, roots);
    store_.setParent(rep, uPar, v, roots);
}

void RBTreeEngine::deleteFixUp(Node* x, Node* xpar, Node* roots[], int v) {
    while (x != roots[v] && store_.getColor(x, v) == BLACK) {
        Node* par = (x != store_.NIL) ? store_.getParent(x, v) : xpar;
        if (par == store_.NIL) break;

        if (x == store_.getLeft(par, v)) {
            Node* w = store_.getRight(par, v);
            if (store_.getColor(w, v) == RED) {
                store_.setColor(w,   BLACK, v, roots);
                store_.setColor(par, RED,   v, roots);
                rotateLeft(par, roots, v);
                xpar = par;
                par  = (x != store_.NIL) ? store_.getParent(x, v) : xpar;
                w    = store_.getRight(par, v);
            }
            if (store_.getColor(store_.getLeft(w,  v), v) == BLACK &&
                store_.getColor(store_.getRight(w, v), v) == BLACK) {
                store_.setColor(w, RED, v, roots);
                x    = par;
                xpar = store_.getParent(x, v);
            } else {
                if (store_.getColor(store_.getRight(w, v), v) == BLACK) {
                    store_.setColor(store_.getLeft(w, v), BLACK, v, roots);
                    store_.setColor(w, RED, v, roots);
                    rotateRight(w, roots, v);
                    par = (x != store_.NIL) ? store_.getParent(x, v) : xpar;
                    w   = store_.getRight(par, v);
                }
                store_.setColor(w,   store_.getColor(par, v), v, roots);
                store_.setColor(par, BLACK, v, roots);
                store_.setColor(store_.getRight(w, v), BLACK, v, roots);
                rotateLeft(par, roots, v);
                x = roots[v];
            }
        } else {
            Node* w = store_.getLeft(par, v);
            if (store_.getColor(w, v) == RED) {
                store_.setColor(w,   BLACK, v, roots);
                store_.setColor(par, RED,   v, roots);
                rotateRight(par, roots, v);
                xpar = par;
                par  = (x != store_.NIL) ? store_.getParent(x, v) : xpar;
                w    = store_.getLeft(par, v);
            }
            if (store_.getColor(store_.getRight(w, v), v) == BLACK &&
                store_.getColor(store_.getLeft(w,  v), v) == BLACK) {
                store_.setColor(w, RED, v, roots);
                x    = par;
                xpar = store_.getParent(x, v);
            } else {
                if (store_.getColor(store_.getLeft(w, v), v) == BLACK) {
                    store_.setColor(store_.getRight(w, v), BLACK, v, roots);
                    store_.setColor(w, RED, v, roots);
                    rotateLeft(w, roots, v);
                    par = (x != store_.NIL) ? store_.getParent(x, v) : xpar;
                    w   = store_.getLeft(par, v);
                }
                store_.setColor(w,   store_.getColor(par, v), v, roots);
                store_.setColor(par, BLACK, v, roots);
                store_.setColor(store_.getLeft(w, v), BLACK, v, roots);
                rotateRight(par, roots, v);
                x = roots[v];
            }
        }
    }
    store_.setColor(x, BLACK, v, roots);
}

Node* RBTreeEngine::searchVer(Node* root, int value, int v) const {
    Node* r = root;
    while (r != store_.NIL) {
        if      (value == r->key) return r;
        else if (value <  r->key) r = store_.getLeft(r, v);
        else                       r = store_.getRight(r, v);
    }
    return store_.NIL;
}

Node* RBTreeEngine::minVer(Node* x, int v) const {
    while (store_.getLeft(x, v) != store_.NIL)
        x = store_.getLeft(x, v);
    return x;
}
