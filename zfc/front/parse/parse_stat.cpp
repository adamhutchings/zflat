#include <front/parse/parse_header.hpp>

void StatementNode::read(std::ifstream& file) {

    Token initial = lex::lex(file);
    Token next;

    switch (initial.type) {
        case TreeComp::FLOW:
            lex::unlex(initial);
            this->inner = new ControlFlowNode();
            this->inner->read(file);
            goto out;
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
            goto out;
        case TreeComp::OBRACE:
            lex::unlex(initial);
            this->inner = new BlockStatementNode();
            this->inner->read(file);
            goto out;
        case TreeComp::LOOP:
            lex::unlex(initial);
            this->inner = new LoopNode();
            this->inner->read(file);
            goto out;
        case TreeComp::IF:
            lex::unlex(initial);
            this->inner = new IfNode();
            this->inner->read(file);
            goto out;
        case TreeComp::SEMICOLON:
            this->inner = nullptr;
            goto out;
        case TreeComp::SWITCH:
        case TreeComp::FSWITCH:
            lex::unlex(initial);
            this->inner = new SwitchNode();
            this->inner->read(file);
            goto out;
        default:
            ZF_TOK_ERR(initial, "control flow statement, value, or '{'");
    }

out:

    // Make sure to parse a trailing semicolon, except for block statements
    switch (initial.type) {
        case OBRACE:
        case LOOP:
        case IF:
        case SWITCH:
        case FSWITCH:
        case SEMICOLON:
            break;
        default:
            expect(file, SEMICOLON);
    }

    if (this->inner != nullptr) {
        this->line = this->inner->line;
    } else {
        this->line = initial.line;
    }

}

