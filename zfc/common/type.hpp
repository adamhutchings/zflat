/**
 * type - types of expressions.
 */

#pragma once

#include <string>
#include <vector>

#include <common/operator.hpp>
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

enum TypeType {
    TT_BUILTIN,
    TT_ENUM,
};

struct Type {
    TypeType type_flavor; // haha
    std::string to_human_str();
    std::string to_output_str();
    bool operator==(Type p);
    BuiltinType primitive = MAX_INVALID; // can be MAX_INVALID
    unsigned int indirection = 0; // how many levels of array this is
    bool ref = false;
    inline Type(BuiltinType t=MAX_INVALID) { primitive = t; }
    bool operator!=(Type t);
    Type deref(); // dereference one type
};

struct Enum : public Type {
    std::vector<void*> values;
    std::string name;
    bool bitfield = false;
    inline Enum(std::string n) : name(n) { type_flavor = TT_ENUM; }
    inline Type underlying_type() { return bitfield ? UINT : UCHAR; }
};

void parse_type(std::ifstream& file, Type* itype);

extern std::vector<Type*> user_types; // All user-defined types
