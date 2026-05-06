#include "io/CommandParser.hpp"
#include <sstream>

Command CommandParser::parse(const std::string& line) const {
    Command cmd;
    std::istringstream ss(line);
    std::string token;
    ss >> token;

    if (token == "INC") {
        cmd.type = CommandType::INC;
        ss >> cmd.arg1;
        cmd.hasArg1 = true;
    } else if (token == "REM") {
        cmd.type = CommandType::REM;
        ss >> cmd.arg1;
        cmd.hasArg1 = true;
    } else if (token == "IMP") {
        cmd.type    = CommandType::IMP;
        cmd.hasArg1 = static_cast<bool>(ss >> cmd.arg1);
    } else if (token == "SUC") {
        cmd.type = CommandType::SUC;
        ss >> cmd.arg1 >> cmd.arg2;
        cmd.hasArg1 = true;
    }

    return cmd;
}
