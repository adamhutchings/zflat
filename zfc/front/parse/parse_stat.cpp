#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void StatementNode::read(std::ifstream& file) {

    Token initial = lex::lex(file);
    Token next;

    switch (initial.type) {
        case TreeComp::FLOW:
            lex::unlex(initial);
            this->inner = new ControlFlowNode();
            this->inner->read(file);
            return;
        case TreeComp::IDENTIFIER:
        case TreeComp::LITERAL:
            // Either expression or declaration.
            bool decl;
            next = lex::lex(file);
            decl = (next.type == TreeComp::COLON);
            lex::unlex(next);
            lex::unlex(initial);
            if (decl)
                this->inner = new VarDeclNode();
            else
                this->inner = new ExprNode();
            this->inner->read(file);
            return;
        case TreeComp::OBRACE:
            lex::unlex(initial);
            this->inner = new BlockStatementNode();
            this->inner->read(file);
            return;
        default:
            ZF_TOK_ERR(initial, "control flow statement, identifier, or '{'");
    }

    // Make sure to parse a trailing semicolon, except for block statements
    if (initial.type != TreeComp::OBRACE) {
        Token sc = lex::lex(file);
        if (sc.type != TreeComp::SEMICOLON)
            ZF_TOK_ERR(sc, "semicolon");
    }

}

