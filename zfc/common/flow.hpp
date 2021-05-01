/**
 * flow - flow control statements
 */

#pragma once

#include <string>

enum ControlFlow {
    BREAK,
    CONTINUE,
    RETURN,
    FLOW_MAX_INVALID,
};

inline ControlFlow strToFlow(std::string in) {
    if (in == "break") return BREAK;
    else if (in == "continue") return CONTINUE;
    else if (in == "return") return RETURN;
    return FLOW_MAX_INVALID;
}

inline std::string flowToStr(ControlFlow in) {
    if (in == BREAK) return "break";
    else if (in == CONTINUE) return "continue";
    else if (in == RETURN) return "return";
    return "!!INVALID_CONTROL!!";
}
