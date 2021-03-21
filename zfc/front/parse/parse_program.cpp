#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void ProgramNode::read(std::ifstream& file) {

    while (1) {

        auto endcheck = lex::lex(file);

        if (endcheck.type == TEOF)
            break;

        lex::unlex(endcheck);

        // Quick and dirty function definition check - check if second token is open paren
        auto first = lex::lex(file);
        auto opn = lex::lex(file);

        if (opn.type == TreeComp::OPAREN) {
            lex::unlex(opn);
            lex::unlex(first);
            FunctionNode* node = new FunctionNode();
            node->read(file);
            this->components.push_back(node);
        } else {
            lex::unlex(opn);
            lex::unlex(first);
            StatementNode* node = new StatementNode();
            node->read(file);
            this->components.push_back(node);
        }

    }

}
