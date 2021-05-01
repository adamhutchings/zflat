#include <front/parse/parse_header.hpp>

void BlockStatementNode::read(std::ifstream& file) {

    Token tok = lex::lex(file);

    if (tok.type != OBRACE) {
        ZF_TOK_ERR(tok, "'{'");
    }

    this->line = tok.line;

    sym::enter_scope();

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

    sym::exit_scope();

}
