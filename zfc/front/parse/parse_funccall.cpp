#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void FuncCallNode::read(std::ifstream& file) {

    Token name = lex::lex(file);

    if (name.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(name, "identifier");
    } else {
        this->name = name.str;
    }
    Token opn = lex::lex(file);
    if (opn.type != TreeComp::OPAREN) {
        ZF_TOK_ERR(opn, "'('");
    }

    // Now we parse expressions, expect commas between them, and a closing paren at the end.
    ExprNode* expr = new ExprNode();
    while (1) {
        expr->read(file);
        this->args.push_back(expr);
        Token sep = lex::lex(file);
        if (sep.type == TreeComp::COMMA) /* go on to the next */;
        else if (sep.type == TreeComp::CPAREN)
            break; // end of arg list
        else
            ZF_TOK_ERR(sep, "',' or ')'");
    }

}
