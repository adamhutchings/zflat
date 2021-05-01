#include <front/parse/parse_header.hpp>

void CaseNode::read(std::ifstream& file) {

    expect(file, CASE);

    this->lit = expect(file, LITERAL).str;

    expect(file, COLON);

    while (1) {
        auto end = lex::lex(file);
        if (end.type == CASE || end.type == CBRACE) {
            lex::unlex(end);
            break;
        }
        lex::unlex(end);
        StatementNode* sn = new StatementNode();
        sn->read(file);
        this->statements.push_back(sn);
    }

}
