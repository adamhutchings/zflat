#include <fstream>

#include <ast/ast.hpp>
#include <back/dtype.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void CaseNode::read(std::ifstream& file) {

    auto cas = lex::lex(file);
    if (cas.type != CASE) {
        ZF_TOK_ERR(cas, "case");
    }

    auto lit = lex::lex(file);
    if (lit.type != LITERAL) {
        ZF_TOK_ERR(lit, "literal value");
    }

    auto col = lex::lex(file);
    if (lit.type != COLON) {
        ZF_TOK_ERR(lit, ":");
    }

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
