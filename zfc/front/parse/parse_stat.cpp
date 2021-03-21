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
            // Either expression or declaration.
            bool decl;
            decl = (next.type == TreeComp::DOT);
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
            ZF_ERROR("Expected control flow statement, identifier, or '{' "
            "on line %u, found \"%s\".", initial.line, initial.raw_content());
    }
}

