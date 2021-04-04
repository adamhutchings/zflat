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
    if (in == "continue") return CONTINUE;
    if (in == "return") return RETURN;
    return FLOW_MAX_INVALID;
}

inline std::string flowToStr(ControlFlow in) {
    if (in == BREAK) return "break";
    if (in == CONTINUE) return "continue";
    if (in == RETURN) return "return";
    return "!!INVALID_CONTROL!!";
}
