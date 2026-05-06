#pragma once

enum class CommandType { INC, REM, IMP, SUC, UNKNOWN };

struct Command {
    CommandType type    = CommandType::UNKNOWN;
    int         arg1    = 0;
    int         arg2    = 0;
    bool        hasArg1 = false;
};
