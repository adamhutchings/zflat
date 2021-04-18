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

BuiltinType zStrToType(std::string in);
std::string typeToZStr(BuiltinType in);

BuiltinType cStrToType(std::string in);
std::string typeToCStr(BuiltinType in);

struct Type {
    std::string to_human_str();
    std::string to_output_str();
    bool operator==(BuiltinType p);
    BuiltinType primitive; // can be MAX_INVALID
    inline Type(BuiltinType t=MAX_INVALID) { primitive = t; }
    bool operator!=(Type t);
};

Type parse_type(std::ifstream& file);
