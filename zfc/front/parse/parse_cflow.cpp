#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void ControlFlowNode::read(std::ifstream& file) {
    Token tok = lex::lex(file);
    if (tok.type != TreeComp::FLOW)
        ZF_TOK_ERR(tok, "break, continue, or return");
    this->statement = tok.str;
    Token next = lex::lex(file);
    if (next.type == TreeComp::SEMICOLON) {
        this->expression = NULL;
    } else {
        this->expression->read(file);
    }
}
