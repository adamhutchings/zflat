#include <fstream>

#include <ast/ast.hpp>
#include  <back/dtype.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void IfNode::read(std::ifstream& file) {

    auto ift = lex::lex(file);
    if (ift.type != IF) {
        ZF_TOK_ERR(ift, "if");
    }

    this->line = ift.line;

    auto op = lex::lex(file);
    if (op.type != OPAREN) {
        ZF_TOK_ERR(op, "'('");
    }

    this->expr = new ExprNode();
    this->expr->read(file);
    if (get_type(this->expr) != BOOL) {
        ZF_ERROR("line %d: if-condition must be of type bool, was of type %s instead",
        this->expr->line, get_type(this->expr).to_human_str().c_str());
    }

    auto cp = lex::lex(file);
    if (cp.type != CPAREN) {
        ZF_TOK_ERR(cp, "')'");
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