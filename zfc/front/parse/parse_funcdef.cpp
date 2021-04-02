#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void FunctionNode::read(std::ifstream& file) {
    
    Token name = lex::lex(file);

    if (name.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(name, "identifier");
    }

    this->symbol = new sym::Function(name.str);
    this->symbol->lineno = name.line;

    Token opn = lex::lex(file);

    if (opn.type != TreeComp::OPAREN) {
        ZF_TOK_ERR(opn, "'('");
    }

    // for while loop, start reading at 1st arg
    lex::unlex(opn);

    while (true) {
        // Loop through to get function arguments
        Token peek = lex::lex(file);
        if (peek.type == TreeComp::CPAREN)
            break;
        else if (peek.type == TreeComp::OPAREN) {
            Token peekc = lex::lex(file);
            if (peekc.type == TreeComp::CPAREN)
                break;
            else {
                lex::unlex(peekc);
            }
        }
        else if (peek.type == TreeComp::COMMA) /* next */;
        else
            ZF_TOK_ERR(peek, "',' or ')'");
        VarDeclNode* node = new VarDeclNode();
        node->read(file);
        this->symbol->args.push_back(sym::Type(node->var->type));
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

    this->symbol->ret = sym::Type(ret.str);

    // Now, parse the rest as a block statement
    this->body = new BlockStatementNode();
    this->body->read(file);

}
