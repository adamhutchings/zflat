/**
 * type - types of expressions.
 */

#pragma once

#include <string>

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
    VA_TYPE,
    MAX_INVALID,
};

Type strToType(std::string in);
std::string typeToStr(Type in);
