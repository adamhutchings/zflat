#include <front/parse/parse_header.hpp>

void SwitchNode::read(std::ifstream& file) {

    auto sw = lex::lex(file);
    if (sw.type == SWITCH) {
        this->fswitch = false;
    } else if (sw.type == FSWITCH) {
        this->fswitch = true;
    } else {
        ZF_TOK_ERR(sw, "switch or fswitch");
    }

    expect(file, OPAREN);

    this->expr = new ExprNode();
    this->expr->read(file);

    expect(file, CPAREN);

    expect(file, OBRACE);

    while (1) {
        auto cb = lex::lex(file);
        if (cb.type == CBRACE) {
            break;
        }
        lex::unlex(cb);
        auto cn = new CaseNode();
        cn->read(file);
        this->cases.push_back(cn);
    }

    this->line = sw.line;

}
