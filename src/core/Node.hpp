#pragma once
#include <vector>
#include <utility>

enum Color { RED, BLACK };

static const int MAX_VERSIONS = 100;
static const int D = 1000;

struct Node {
    int   key;
    Node* origLeft;
    Node* origRight;
    Node* origParent;
    Color origColor;
    std::vector<std::pair<int, Node*>>  leftMods;
    std::vector<std::pair<int, Node*>>  rightMods;
    std::vector<std::pair<int, Node*>>  parentMods;
    std::vector<std::pair<int, Color>>  colorMods;
};
