#include <front/parse/parse_header.hpp>

enum TopLevelDecl {
    TL_VAR,
    TL_FN,
    TL_ENUM,
};

TopLevelDecl check_fn(std::ifstream& file) {

    // check if next is a function definition

    // Quick and dirty function definition check - check if second token is open paren
    // and fourth is colon
    auto first = lex::lex(file);
    auto opn = lex::lex(file);
    auto un = lex::lex(file);
    auto cl = lex::lex(file);

    bool fn = first.type == TreeComp::IDENTIFIER
    && opn.type == TreeComp::OPAREN
    && ( ( un.type == TreeComp::IDENTIFIER
    && cl.type == TreeComp::COLON )
        || (un.type == TreeComp::TYPENAME || un.type == TreeComp::REF)
    );

    bool pless = first.type == TreeComp::IDENTIFIER
    && opn.type == TreeComp::OPAREN
    && un.type == TreeComp::CPAREN
    && (cl.type == TreeComp::COLON || cl.type == TreeComp::OBRACE);

    lex::unlex(cl);
    lex::unlex(un);
    lex::unlex(opn);
    lex::unlex(first);

    if (fn || pless) return TL_FN;
    if (first.type == ENUM || first.type == BITFIELD) return TL_ENUM;
    return TL_VAR;

}

void ProgramNode::read(std::ifstream& file) {

    sym::enter_scope(); // import scope

    while (1) {
        auto use = lex::lex(file);
        if (use.type != USE) {
            // Not a use statement - end use statement parsing.
            lex::unlex(use);
            break;
        }
        lex::unlex(use);
        auto ustmt = new UseNode();
        ustmt->read(file);
        this->imports.push_back(ustmt);
        for (auto sym : ustmt->symtab) {
            sym::add_symbol(sym);
        }
    }

    sym::enter_scope(); // global scope

    while (1) {

        auto endcheck = lex::lex(file);

        if (endcheck.type == TEOF) {
            return;
        }

        lex::unlex(endcheck);

        ProgramSub* sub;

        if (check_fn(file) == TL_FN || endcheck.type == EXTC) {
            sub = new FunctionNode();
        } else if (check_fn(file) == TL_VAR) {
            sub = new StatementNode();
        } else {
            sub = new EnumDeclNode();
        }

        sub->read(file);
        warn_unused_value(sub);

        this->components.push_back(sub);

    }

    sym::exit_scope(); // exit global scope

    sym::exit_scope(); // exit import scope

}
