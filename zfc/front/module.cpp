#include "module.hpp"

/**
 * Note (Adam)
 * TODO: call into proper parse/gen code from elsewhere instead of crudely
 * duplicating them here.
 */

/**
 * SYMBOL FILE FORMAT
 */

/**
 * factorial_i(int): int
 * printf(fmt: char[], __c_va_args): int;
 */

#define ZF_SYMBOL_ERR(...) ZF_ERROR("symbol: " __VA_ARGS__)

#define MIN(x, y) ( (x) < (y) ? (x) : (y) )
namespace mod {

void begin_read(std::ifstream& file) {
    // TODO - read version number, perhaps?
    lex::reset_lexer();
}

sym::Symbol* readsym(std::ifstream& file) {
    sym::Symbol* ret;
    // Use the lexer!
    auto ident = lex::lex(file);
    if (ident.type == TEOF) return nullptr;
    if (ident.type != IDENTIFIER) {
        ZF_SYMBOL_ERR("line %d: no identifier found", ident.line);
    }
    auto det = lex::lex(file);
    // Decide on func call or var
    if (det.type == OPAREN) {
        ret = new sym::Function(ident.str);
        auto fret = static_cast<sym::Function*>(ret);
        fret->name = ident.str;
        // Look for func call args
        int num = 0;
        while (1) {
            // Check for end
            auto endtest = lex::lex(file);
            if (endtest.type == CPAREN) {
                break;
            }
            lex::unlex(endtest);
            if (num != 0) {
                auto cotok = lex::lex(file);
                if (cotok.type != COMMA) {
                    ZF_SYMBOL_ERR("line %d: expected comma", cotok.line);
                }
            }
            auto arg = lex::lex(file);
            if (arg.type != IDENTIFIER && arg.type != TYPENAME) {
                // The lexer might detect it as either.
                ZF_SYMBOL_ERR("line %d: expected identifier", arg.line);
            }
            // Hack - the type parser expects an type, so put back the token
            // AFTER changing its type to TYPENAME.
            arg.type = TYPENAME;
            lex::unlex(arg);
            sym::Variable var(std::string("symbol_read_var_") + std::to_string(num));
            var.type = *parse_type(file);
            fret->args.push_back(var);
            ++num; // always after first arg
        }
        // Parse return type, if any
        auto rettest = lex::lex(file);
        if (rettest.type != COLON) {
            lex::unlex(rettest);
            fret->ret = VOID;
        } else {
            fret->ret = *parse_type(file);
        }
    } else if (det.type == COLON) {
        // Get type name
        ret = new sym::Variable(ident.str);
        auto vret = static_cast<sym::Variable*>(ret);
        vret->type = *parse_type(file);
    } else {
        ZF_SYMBOL_ERR("expected valid line %d", ident.line);
    }
    return ret;
}

void end_read(std::ifstream& file) {
    lex::reset_lexer();
}

void begin_write(std::ofstream& file) {
    // file << "version 0.0.1\n";
    // no version parsing YET
}

void writesym(std::ofstream& file, sym::Symbol* sym) {
    sym::Variable* var = static_cast<sym::Variable*>(sym);
    sym::Function* fun = static_cast<sym::Function*>(sym);
    int ct;
    switch (sym->s_type) {
    case sym::SymType::VAR:
        // Write x: int, for example
        file << var->name << ": " << var->type.to_human_str() << "\n";
        return;
    case sym::SymType::FN:
        if (fun->extc) {
            file << "extc ";
        }
        file << fun->name << " (";
        ct = 0;
        for (auto arg : fun->args) {
            if (ct != 0) {
                file << ", ";
            }
            file << arg.type.to_human_str();
            ++ct;
        }
        file << "): ";
        file << fun->ret.to_human_str();
        file << "\n";
    case sym::SymType::T_ENUM:
        // TODO: do after refactor
    case sym::SymType::T_ENUMSYM:
        ; // better not happen
    }
}

void end_write(std::ofstream& file) {
    // End marker?
}

}
