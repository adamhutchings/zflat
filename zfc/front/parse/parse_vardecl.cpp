#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void VarDeclNode::read(std::ifstream& file) {
    Token init = lex::lex(file);
    if (init.type != TreeComp::IDENTIFIER) {
        ZF_ERROR("expected identifier on line %u, found \"%s\"", init.line, init.raw_content());
    } else {
        this->name = init.str;
    }
    Token next = lex::lex(file);
    if (next.type != TreeComp::COLON) {
        ZF_ERROR("expected ':' on line %u, got \"%s\" instead", next.line, next.str);
    }
    next = lex::lex(file);
    if (next.type != TreeComp::TYPENAME) {
        ZF_ERROR("expected type name on line %u, found \"%s\" instead", next.line, next.str);
    } else {
        this->ntype = next.str;
    }
    next = lex::lex(file);
    // followed by anything valid (end of statement)              (end of function arg list) (next argument)
    if (next.type == TreeComp::SEMICOLON || next.type == TreeComp::CPAREN || next.type == TreeComp::COMMA) {
        this->expr = nullptr;
    } else {
        // parse an expression
        lex::unlex(next);
        this->expr = new ExprNode();
        this->expr->read(file);
    }
}
