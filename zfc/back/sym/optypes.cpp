#include "optypes.hpp"

namespace op {

BuiltinType strToType(std::string str) {
    if (str == "void")     return VOID;
    if (str == "char")     return CHAR;
    if (str == "short")    return SHORT;
    if (str == "int")      return INT;
    if (str == "long")     return LONG;
    if (str == "float")    return FLOAT;
    if (str == "double")   return DOUBLE;
    if (str == "uchar")    return UCHAR;
    if (str == "ushort")   return USHORT;
    if (str == "uint")     return UINT;
    if (str == "ulong")    return ULONG;
    return MAX_BT_INVALID;
}

BuiltinOperator strToOp(std::string str) {
    if (str == "=")        return AEQ;
    if (str == "==")       return EQ;
    if (str == "!=")       return NEQ;
    if (str == "|")        return OR;
    if (str == "&")        return AND;
    if (str == "^")        return XOR;
    if (str == "+")        return ADD;
    if (str == "-")        return SUB;
    if (str == "*")        return MUL;
    if (str == "/")        return DIV;
    if (str == "|=")       return OREQ;
    if (str == "&=")       return ANDEQ;
    if (str == "^=")       return XOREQ;
    if (str == "+=")       return ADDEQ;
    if (str == "-=")       return SUBEQ;
    if (str == "*=")       return MULEQ;
    if (str == "/=")       return DIVEQ;
    return MAX_OP_INVALID;
}

BuiltinType result(BuiltinType left, BuiltinOperator op, BuiltinType right) {

    // invalids
    if (left == VOID || right == VOID) return MAX_BT_INVALID;

    // assignment (augmented or not)
    if (static_cast<int> (op) > static_cast<int> (DIV) || op == AEQ) return left;

    // binary operators
    if (op == EQ || op == NEQ) return BOOL;

    if ( (left == BOOL) != (right == BOOL) ) return MAX_BT_INVALID;

    // Get the largest size
    return left > right ? left : right;

}

}
