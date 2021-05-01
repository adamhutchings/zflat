/**
 * parse_header - general utilities for parsing
 */

#pragma once

#include <fstream>

#include <common/ast.hpp>
#include <common/dtype.hpp>
#include <common/operator.hpp>
#include <front/lex.hpp>
#include <front/module.hpp>
#include <front/trfrags.hpp>
#include <util/error.hpp>

inline std::string get_type_name(TreeComp type) {
    switch (type) {
        case IDENTIFIER: return "identifier";
        case TYPENAME: return "type name";
        case LITERAL: return "literal";
        case OPERATOR: return "operator";
        case SEMICOLON: return "';''";
        case OBRACE: return "'{'";
        case CBRACE: return "'}";
        case OPAREN: return "'('";
        case CPAREN: return "')";
        case OBRACKET: return "'[";
        case CBRACKET: return "']'";
        case COLON: return "':'";
        case COMMA: return "','";
        case DOT: return "'.'";
        case FLOW: return "break, continue, or return";
        case LOOP: return "'loop'";
        case IF: return "'if'";
        case ELSE: return "'else'";
        case SWITCH: return "'switch'";
        case FSWITCH: return "'fswitch'";
        case CASE: return "'case'";
        case EXTC: return "'extc'";
        case VA_ARGS: return "'__c_va_args'";
        case USE: return "'use'";
        case REF: return "'ref'";
        default: return "unknown";
    }
}

/**
 * Expect a token and return it.
 */
inline Token expect(std::ifstream& file, TreeComp type) {
    auto tok = lex::lex(file);
    auto atype = get_type_name(tok.type);
    if (tok.type != type) {
        ZF_TOK_ERR(tok, atype.c_str());
    }
    return tok;
}
