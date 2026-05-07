#pragma once
#include <vector>
#include <variant>

enum Color { RED, BLACK };
enum class Field { LEFT, RIGHT, PARENT, COLOR };

static const int MAX_VERSIONS = 100;
static const int D = 6;    // 2 * p, p = 3 ponteiros (left, right, parent)

struct Node;

struct Mod {
    int   version;
    Field field;
    std::variant<Node*, Color> value;
};

struct Node {
    int   key;
    Node* origLeft;
    Node* origRight;
    Node* origParent;
    Color origColor;
    std::vector<Mod>   mods;
    std::vector<Node*> backPointers; // não versionado — quem aponta para este nó na última versão
};
