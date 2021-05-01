#include <front/parse/parse_header.hpp>

void LoopNode::read(std::ifstream& file) {
    auto loop = lex::lex(file);
    if (loop.type != LOOP) {
        ZF_TOK_ERR(loop, "'loop'");
    }
    this->line = loop.line;
    auto op = lex::lex(file);
    if (op.type != OPAREN) {
        ZF_TOK_ERR(op, "'('");
    }
    this->expr = new ExprNode();
    this->expr->read(file);
    auto cp = lex::lex(file);
    if (cp.type != CPAREN) {
        ZF_TOK_ERR(cp, "')");
    }
    auto co = lex::lex(file);
    if (co.type == COLON) {
        this->pred = new VarDeclNode();
        this->pred->read(file);
    } else {
        lex::unlex(co);
        this->pred = nullptr;
    }
    this->stmt = new StatementNode();
    this->stmt->read(file);
}
