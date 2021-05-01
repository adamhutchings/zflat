#include <front/parse/parse_header.hpp>

void IfNode::read(std::ifstream& file) {

    this->line = expect(file, IF).line;

    expect(file, OPAREN);

    this->expr = new ExprNode();
    this->expr->read(file);
    if (get_type(this->expr) != BOOL) {
        ZF_ERROR("line %d: if-condition must be of type bool, was of type %s instead",
        this->expr->line, get_type(this->expr).to_human_str().c_str());
    }

    expect(file, CPAREN);

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