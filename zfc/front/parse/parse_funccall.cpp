#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void FuncCallNode::read(std::ifstream& file) {

    Token name = lex::lex(file);

    if (name.type != TreeComp::IDENTIFIER) {
        ZF_ERROR("expected name on line %u, found \"%s\"", name.line, name.raw_content());
    } else {
        this->name = name.str;
    }
    Token opn = lex::lex(file);
    if (opn.type != TreeComp::OPAREN) {
        ZF_ERROR("expected '(' on line %u, found \"%s\"", opn.line, opn.raw_content());
    }

    // Now we parse expressions, expect commas between them, and a closing paren at the end.
    ExprNode expr;
    while (1) {
        expr.read(file);
        this->args.push_back(expr);
        Token sep = lex::lex(file);
        if (sep.type == TreeComp::COMMA) /* go on to the next */;
        else if (sep.type == TreeComp::CPAREN)
            break; // end of arg list
        else
            ZF_ERROR("expected ',' or ')' after argument on line %u, found \"%s\" instead", sep.line, sep.raw_content());
    }

}
