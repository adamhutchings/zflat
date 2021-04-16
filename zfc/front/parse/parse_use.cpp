#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void UseNode::read(std::ifstream& file) {

    auto use = lex::lex(file);
    if (use.type != USE) {
        ZF_TOK_ERR(use, "'use'");
    }

    // Read the path into a string.
    // TODO - handle prefixes (like mylib.file.path)
    // TODO - use expr.ident syntax rather than the manual version right now
    std::string path = "bin/symbol/";
    while (1) {
        auto part = lex::lex(file);
        if (part.type != IDENTIFIER) {
            ZF_TOK_ERR(part, "identifier");
        }
        // Add to path
        path += part.str;
        auto end_or_dot = lex::lex(file);
        if (end_or_dot.type == SEMICOLON) {
            break; // end of use statement
        }
        if (end_or_dot.type == DOT) {
            // folder break in path
            path += "/";
        } else {
            ZF_TOK_ERR(end_or_dot, "';' or '.'");
        }
    }

    path += ".zf";

    // TODO - read symbol table

}
