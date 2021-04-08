#include "operator.hpp"

namespace op {

int binding_order(Operator a, Operator b) {
    int diff = (a / 8) - (b - 8);
    return (diff == 0) ? 0 : (diff < 0) ? -1 : 1;
}

Operator strToOp(std::string in) {
    if (in == "=") return EQUALS;
    if (in == "+=") return ADDEQ;
    if (in == "-=") return SUBEQ;
    if (in == "*=") return MULEQ;
    if (in == "/=") return DIVEQ;
    if (in == "%=") return REMEQ;
    if (in == "&=") return ANDEQ;
    if (in == "|=") return OREQ;
    if (in == "^=") return XOREQ;
    if (in == "==") return EQ;
    if (in == "!=") return NEQ;
    if (in == ">") return GREATER;
    if (in == "<") return LESSER;
    if (in == ">=") return GREQ;
    if (in == "<=") return LESSEQ;
    if (in == "+") return ADD;
    if (in == "-") return SUB;
    if (in == "*") return MUL;
    if (in == "/") return DIV;
    if (in == "%") return REM;
    if (in == "&") return AND;
    if (in == "|") return OR;
    if (in == "^") return XOR;
    return INVALID;
}

std::string opToStr(Operator in) {
    if (in == EQUALS) return "=";
    if (in == ADDEQ) return "+=";
    if (in == SUBEQ) return "-=";
    if (in == MULEQ) return "*=";
    if (in == DIVEQ) return "/=";
    if (in == REMEQ) return "%=";
    if (in == ANDEQ) return "&=";
    if (in == OREQ) return "|=";
    if (in == XOREQ) return "^=";
    if (in == EQ) return "==";
    if (in == NEQ) return "!=";
    if (in == GREATER) return ">";
    if (in == LESSER) return "<";
    if (in == GREQ) return ">=";
    if (in == LESSEQ) return "<=";
    if (in == ADD) return "+";
    if (in == SUB) return "-";
    if (in == MUL) return "*";
    if (in == DIV) return "/";
    if (in == REM) return "%";
    if (in == AND) return "&";
    if (in == OR) return "|";
    if (in == XOR) return "^";
    return "!!INVALID_OPERATOR!!";
}

bool is_assign(Operator a) {
    return (a / OP_GROUP_SIZE) == 0;
}

bool is_bool(Operator a) {
    int group = a / OP_GROUP_SIZE;
    return (a == 1) || (a == 2);
}

}
