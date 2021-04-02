#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void FuncCallNode::read(std::ifstream& file) {

    Token name = lex::lex(file);

    if (name.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(name, "identifier");
    } else {
        this->ref = sym::resolve(name.str);
        if (this->ref == nullptr) {
            ZF_ERROR("could not resolve function \"%s\" on line %d", name.raw_content(), name.line);
        }
    }
    Token opn = lex::lex(file);
    if (opn.type != TreeComp::OPAREN) {
        ZF_TOK_ERR(opn, "'('");
    }

    Token ctok = lex::lex(file);

    if (ctok.type == TreeComp::CPAREN)
        return; // no arguments

    lex::unlex(ctok);

    while (1) {
        ExprNode* exp = new ExprNode();
        exp->read(file);
        this->args.push_back(exp);
        Token peek = lex::lex(file);
        if (peek.type == TreeComp::COMMA) continue;
        else if (peek.type == TreeComp::CPAREN) return;
        else
            ZF_TOK_ERR(peek, "',' or ')'");
    }

}
