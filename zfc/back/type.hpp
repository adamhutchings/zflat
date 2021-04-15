/**
 * type - types of expressions.
 */

#pragma once

#include <string>

#include <back/operator.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

enum BuiltinType {
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
    VA_TYPE,
    MAX_INVALID,
};

struct Type {
    std::string to_str();
    bool operator==(BuiltinType p);
    BuiltinType primitive; // can be MAX_INVALID
    inline Type(BuiltinType t=MAX_INVALID) { primitive = t; }
    bool operator!=(Type t);
};

Type parse_type(std::ifstream& file);
