#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void FunctionNode::read(std::ifstream& file) {
    
    Token name = lex::lex(file);

    if (name.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(name, "identifier");
    }

    this->name = name.str;

    Token opn = lex::lex(file);

    if (opn.type != TreeComp::OPAREN) {
        ZF_TOK_ERR(opn, "'('");
    }
    while (true) {
        // Loop through to get function arguments
        Token peek = lex::lex(file);
        if (peek.type == TreeComp::CPAREN)
            break;
        else if (peek.type == TreeComp::COMMA) /* next */;
        else
            ZF_TOK_ERR(peek, "',' or ')'");
        VarDeclNode* node = new VarDeclNode();
        node->read(file);
        this->args.push_back(node);
    }

    // Now we've passed the function argument list, get return type
    Token cln = lex::lex(file);
    if (cln.type != TreeComp::COLON) {
        ZF_TOK_ERR(cln, "':'");
    }

    Token ret = lex::lex(file);
    if (ret.type != TreeComp::TYPENAME) {
        ZF_TOK_ERR(ret, "type name");
    }

    this->ret_type = ret.str;

    // Now, parse the rest as a block statement
    this->body = new BlockStatementNode();
    this->body->read(file);

}
