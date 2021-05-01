#include <front/parse/parse_header.hpp>

void LoopNode::read(std::ifstream& file) {
    this->line = expect(file, LOOP).line;
    expect(file, OPAREN);
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
