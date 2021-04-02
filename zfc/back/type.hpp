/**
 * type - types of expressions.
 */

#pragma once

#include <ast/ast.hpp>
#include <back/operator.hpp>
#include <util/error.hpp>

enum Type {
    // From smallest to largest
    BOOL,
    CHAR,
    UCHAR,
    SHORT,
    USHORT,
    INT,
    UINT,
    FLOAT,
    DOUBLE,
    LONG,
    ULONG,
    VOID,
};

Type get_type(ExprNode* expr);
