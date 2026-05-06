#pragma once
#include "Node.hpp"
#include <vector>

class FatNodeStore {
public:
    FatNodeStore();
    ~FatNodeStore();

    Node* NIL;

    Node* makeNode(int key);

    Node* getLeft  (Node* n, int v) const;
    Node* getRight (Node* n, int v) const;
    Node* getParent(Node* n, int v) const;
    Color getColor (Node* n, int v) const;

    Node* setLeft  (Node* n, Node* val, int v, Node* roots[]);
    Node* setRight (Node* n, Node* val, int v, Node* roots[]);
    Node* setParent(Node* n, Node* val, int v, Node* roots[]);
    Node* setColor (Node* n, Color val, int v, Node* roots[]);

private:
    std::vector<Node*> allNodes_;

    template<typename T>
    T lastLE(const std::vector<std::pair<int, T>>& mods, int v, T def) const {
        int lo = 0, hi = (int)mods.size() - 1, idx = -1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (mods[mid].first <= v) { idx = mid; lo = mid + 1; }
            else hi = mid - 1;
        }
        return idx >= 0 ? mods[idx].second : def;
    }

    int   totalMods(Node* n) const;
    Node* handleOverflow(Node* old, int v, Node* roots[]);
};
