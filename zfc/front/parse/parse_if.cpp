#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void IfNode::read(std::ifstream& file) {

    auto ift = lex::lex(file);
    if (ift.type != IF) {
        ZF_TOK_ERR(ift, "if");
    }

    auto op = lex::lex(file);
    if (op.type != OPAREN) {
        ZF_TOK_ERR(op, "'('");
    }

    this->expr = new ExprNode();
    this->expr->read(file);

    auto cp = lex::lex(file);
    if (cp.type != CPAREN) {
        ZF_TOK_ERR(op, "')'");
    }

    this->stmt = new StatementNode();
    this->stmt->read(file);

    auto pot = lex::lex(file);
    if (pot.type == ELSE) {
        this->else_block = new StatementNode();
        this->else_block->read(file);
    } else {
        lex::unlex(pot);
        this->else_block = nullptr;
    }

}