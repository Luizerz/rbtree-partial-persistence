#pragma once
#include <ostream>
#include <iostream>
#include <string>

inline void emit(const std::string& s, std::ostream& fileOut) {
    fileOut   << s << "\n";
    std::cout << s << "\n";
}
