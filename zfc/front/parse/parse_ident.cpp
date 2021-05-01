#include <front/parse/parse_header.hpp>

/**
 * This file is to get the name of an identifier as a string.
 */

std::string get_ident_name(std::ifstream& file) {

    int ct = 0;

    std::string ret = "";

    while (1) {

        if (ct != 0) {
            // Expect dot
            auto dot = lex::lex(file);
            if (dot.type != DOT) {
                lex::unlex(dot);
                break;
            }
            ret += sym::scopesep;
        }

        auto ide = lex::lex(file);
        if (ide.type != IDENTIFIER) {
            ZF_TOK_ERR(ide, "identifier");
        }
        ret += ide.str;

        ++ct;

    }

    return ret;

}
