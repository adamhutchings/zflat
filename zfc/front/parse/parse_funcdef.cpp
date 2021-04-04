#include <fstream>

#include <ast/ast.hpp>
#include <back/operator.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void FunctionNode::read(std::ifstream& file) {

    // argument scope
    sym::enter_scope();
    
    Token name = lex::lex(file);
    this->line = name.line;

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
        // Make sure it isn't a duplicate symbol
        for (auto vd : this->symbol->args) {
            if (vd.name == node->var->name) {
                ZF_ERROR("line %d: duplicate argument %s", node->line, node->var->name.c_str());
            }
        }
        this->symbol->args.push_back(*node->var);
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

    this->symbol->ret = strToType(ret.str);

    sym::add_global_symbol(this->symbol);

    sym::set_function(this->symbol);

    // Now, parse the rest as a block statement
    this->body = new BlockStatementNode();
    this->body->read(file);

    // exit argument scope
    sym::exit_scope();

    sym::set_function(nullptr);

}
