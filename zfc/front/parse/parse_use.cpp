#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <front/module.hpp>
#include <util/error.hpp>

void UseNode::read(std::ifstream& file) {

    auto use = lex::lex(file);
    if (use.type != USE) {
        ZF_TOK_ERR(use, "'use'");
    }

    // Read the path into a string.
    // TODO - handle prefixes (like mylib.file.path)
    // TODO - use expr.ident syntax rather than the manual version right now
    std::string path = "bin/symbol/", symbol_prefix = "";
    while (1) {
        auto part = lex::lex(file);
        if (part.type != IDENTIFIER) {
            ZF_TOK_ERR(part, "identifier");
        }
        // Add to path
        path += part.str, symbol_prefix += part.str;
        auto end_or_dot = lex::lex(file);
        if (end_or_dot.type == SEMICOLON) {
            break; // end of use statement
        }
        if (end_or_dot.type == DOT) {
            // folder break in path
            path += "/", symbol_prefix += "$$$";
        } else {
            ZF_TOK_ERR(end_or_dot, "';' or '.'");
        }
    }

    path += ".symbol";

    std::ifstream symbol_file;
    symbol_file.open(path);
    if (!symbol_file) {
        /**
         * This is super annoying in general.
         * TODO - fix this so that if this is encountered, compile the referenced
         * file so that the symbol file becomes available.
         */
        ZF_ERROR("line %d: could not find symbol file %s", use.line, path.c_str());
    }

    sym::Symbol* sym;
    mod::begin_read(symbol_file);
    while ( (sym = mod::readsym(symbol_file)) != nullptr ) {
        sym::add_symbol(sym);
        sym->name = symbol_prefix + sym->name;
    }
    mod::end_read(symbol_file);

}
