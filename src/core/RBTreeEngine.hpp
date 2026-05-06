#pragma once
#include "FatNodeStore.hpp"

class RBTreeEngine {
public:
    explicit RBTreeEngine(FatNodeStore& store);

    void  rotateLeft (Node* x,              Node* roots[], int v);
    void  rotateRight(Node* x,              Node* roots[], int v);
    void  insertFixUp(Node* z,              Node* roots[], int v);
    void  deleteFixUp(Node* x, Node* xpar, Node* roots[], int v);
    void  transplant (Node* u, Node* rep,   Node* roots[], int v);
    Node* searchVer  (Node* root, int value, int v) const;
    Node* minVer     (Node* x, int v) const;

private:
    FatNodeStore& store_;
};
