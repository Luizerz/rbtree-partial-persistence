#include "query/SuccessorFinder.hpp"
#include <climits>

namespace TreeQuery {

    static void succHelper(Node* n, int x, int v, FatNodeStore& store, int& best) {
        if (n == store.NIL) return;
        if (n->key > x) {
            if (n->key < best) best = n->key;
            succHelper(store.getLeft(n, v), x, v, store, best);
        } else {
            succHelper(store.getRight(n, v), x, v, store, best);
        }
    }

    int successor(Node* root, int x, int v, FatNodeStore& store) {
        int best = INT_MAX;
        succHelper(root, x, v, store, best);
        return best;
    }

}
