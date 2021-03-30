/**
 * optypes - the types that operators take with given left and right operands,
 * and also the expression validation code.
 */

#pragma once

#include <string>

namespace op {

enum BuiltinType {
    VOID,
    BOOL,
    CHAR,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    UCHAR,
    USHORT,
    UINT,
    ULONG,
    MAX_BT_INVALID
};

enum BuiltinOperator {
    AEQ,        // =
    EQ,         // ==
    NEQ,        // !=
    OR,         // |
    AND,        // &
    XOR,        // ^
    ADD,        // +
    SUB,        // -
    MUL,        // *
    DIV,        // /
    OREQ,       // |=
    ANDEQ,      // &=
    XOREQ,      // ^=
    ADDEQ,      // +=
    SUBEQ,      // -=
    MULEQ,      // *=
    DIVEQ,      // /=
    MAX_OP_INVALID
};

BuiltinType strToType(std::string str);

BuiltinOperator strToOp(std::string op);

BuiltinType result(BuiltinType left, BuiltinOperator op, BuiltinType right);

}
