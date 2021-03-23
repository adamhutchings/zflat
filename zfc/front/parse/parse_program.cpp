#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

bool check_fn(std::ifstream& file) {

    // check if next is a function definition

    // Quick and dirty function definition check - check if second token is open paren
    // and fourth is colon
    auto first = lex::lex(file);
    auto opn = lex::lex(file);
    auto un = lex::lex(file);
    auto cl = lex::lex(file);

    return (first.type == TreeComp::IDENTIFIER
    && opn.type == TreeComp::OPAREN
    && un.type == TreeComp::IDENTIFIER
    && cl.type == TreeComp::COLON);

    lex::unlex(cl);
    lex::unlex(un);
    lex::unlex(opn);
    lex::unlex(first);

}

void ProgramNode::read(std::ifstream& file) {

    while (1) {

        auto endcheck = lex::lex(file);

        if (endcheck.type == TEOF)
            break;

        lex::unlex(endcheck);

        ProgramSub* sub;

        if (check_fn(file)) {
            sub = new FunctionNode();
        } else {
            sub = new StatementNode();
        }
        sub->read(file);
        this->components.push_back(sub);

    }

}
