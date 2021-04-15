#include "type.hpp"

namespace {

BuiltinType strToType(std::string in) {
    if (in == "bool") return BOOL;
    if (in == "char") return CHAR;
    if (in == "uchar") return UCHAR;
    if (in == "short") return SHORT;
    if (in == "ushort") return USHORT;
    if (in == "int") return INT;
    if (in == "uint") return UINT;
    if (in == "float") return FLOAT;
    if (in == "double") return DOUBLE;
    if (in == "long") return LONG;
    if (in == "ulong") return ULONG;
    if (in == "void") return VOID;
    return MAX_INVALID;
}

std::string typeToStr(BuiltinType in) {
    if (in == BOOL) return "_Bool";
    if (in == CHAR) return "signed char";
    if (in == UCHAR) return "unsigned char";
    if (in == SHORT) return "short";
    if (in == USHORT) return "unsigned short";
    if (in == INT) return "long";
    if (in == UINT) return "unsigned long";
    if (in == FLOAT) return "float";
    if (in == DOUBLE) return "double";
    if (in == LONG) return "long long";
    if (in == ULONG) return "unsigned long long";
    if (in == VOID) return "void";
    if (in == VA_TYPE) return "...";
    return "!!INVALID_TYPE!!";
}

}

bool Type::operator==(BuiltinType p) {
    return this->primitive == p;
}

bool Type::operator!=(Type p) {
    return this->primitive != p.primitive;
}

Type parse_type(std::ifstream& file) {
    Type ret;
    // Change this later, if need be.
    auto type = lex::lex(file);
    if (type.type != TYPENAME) {
        ZF_ERROR("line %d: expected type name, found \"%s\"", type.line, type.raw_content());
    }
    ret.primitive = strToType(type.str);
    if (ret.primitive == MAX_INVALID) {
        ZF_ERROR("line %d: type name \"%s\" not recognized", type.line, type.raw_content());
    }
    return ret;
}

std::string Type::to_str() {
    return typeToStr(this->primitive);
}
