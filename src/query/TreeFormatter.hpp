#pragma once
#include "core/FatNodeStore.hpp"
#include <sstream>

namespace TreeQuery {
    void inorder(Node* root, int depth, int v, FatNodeStore& store, std::ostringstream& buf);
}
