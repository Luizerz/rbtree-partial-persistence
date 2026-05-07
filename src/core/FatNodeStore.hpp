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

    // Busca binária pelo corte de versão, depois varredura reversa pelo campo f
    template<typename T>
    T lastLE(const std::vector<Mod>& mods, int v, Field f, T def) const {
        int lo = 0, hi = (int)mods.size() - 1, cut = -1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (mods[mid].version <= v) { cut = mid; lo = mid + 1; }
            else hi = mid - 1;
        }
        for (int i = cut; i >= 0; --i)
            if (mods[i].field == f)
                return std::get<T>(mods[i].value);
        return def;
    }

    void  removeBackPtr(Node* from, Node* n);
    int   totalMods(Node* n) const;
    Node* handleOverflow(Node* old, int v, Node* roots[]);
};
