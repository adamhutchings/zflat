#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void BlockStatementNode::read(std::ifstream& file) {

    Token tok = lex::lex(file);

    if (tok.type != OBRACE) {
        ZF_TOK_ERR(tok, "'{'");
    }
    while (true) {
        // Parse until we hit a closing brace
        Token pclose = lex::lex(file);
        if (pclose.type == CBRACE) {
            break;
        } else {
            lex::unlex(pclose);
            StatementNode* st = new StatementNode();
            st->read(file);
            this->statements.push_back(st);
        }
    }

}
