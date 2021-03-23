#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void VarDeclNode::read(std::ifstream& file) {
    Token init = lex::lex(file);
    if (init.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(init, "identifier");
    } else {
        this->name = init.str;
    }
    Token next = lex::lex(file);
    if (next.type != TreeComp::COLON) {
        ZF_TOK_ERR(next, "':'");
    }
    next = lex::lex(file);
    if (next.type != TreeComp::TYPENAME) {
        ZF_TOK_ERR(next, "type name");
    } else {
        this->ntype = next.str;
    }
    next = lex::lex(file);
    // followed by anything valid (end of statement)              (end of function arg list) (next argument)
    if (next.type == TreeComp::SEMICOLON || next.type == TreeComp::CPAREN || next.type == TreeComp::COMMA) {
        this->expr = nullptr;
        lex::unlex(next);
    } else if (next.type == TreeComp::OPERATOR) {
        // parse an expression (after an equals sign, check for this later)
        this->expr = new ExprNode();
        this->expr->read(file);
    }
    next = lex::lex(file);
    if (next.type != TreeComp::SEMICOLON) {
        ZF_TOK_ERR(next, "';'");
    }
}
