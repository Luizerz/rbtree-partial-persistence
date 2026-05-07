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
    T lastLE(const std::vector<Mod>& mods, int v, Field f, T def) const {
        for (int i = (int)mods.size() - 1; i >= 0; --i)
            if (mods[i].version <= v && mods[i].field == f)
                return std::get<T>(mods[i].value);
        return def;
    }

    void  removeBackPtr(Node* from, Node* n);
    int   totalMods(Node* n) const;
    Node* handleOverflow(Node* old, int v, Node* roots[]);
};
