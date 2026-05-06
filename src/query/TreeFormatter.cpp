#include "query/TreeFormatter.hpp"

namespace TreeQuery {
    void inorder(Node* root, int depth, int v, FatNodeStore& store, std::ostringstream& buf) {
        if (root == store.NIL) return;
        inorder(store.getLeft(root, v), depth + 1, v, store, buf);
        buf << root->key << "," << depth << ","
            << (store.getColor(root, v) == RED ? "R" : "N") << " ";
        inorder(store.getRight(root, v), depth + 1, v, store, buf);
    }
}
