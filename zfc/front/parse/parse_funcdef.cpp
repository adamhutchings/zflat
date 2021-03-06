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

    bool extc;

    if (name.type == TreeComp::EXTC) {
        extc = true;
        name = lex::lex(file);
    } else {
        extc = false;
    }

    if (name.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(name, "identifier");
    }

    this->symbol = new sym::Function(name.str);
    this->symbol->lineno = name.line;
    this->symbol->extc = extc;

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
        // Check for va_args
        auto va = lex::lex(file);
            VarDeclNode* node = new VarDeclNode();
        if (va.type == VA_ARGS) {
            node->var = new sym::Variable("");
            node->var->type = VA_TYPE;
        } else {
            lex::unlex(va);
            node->read(file);
        }
        // Make sure it isn't a duplicate symbol
        // Also check to make sure the va_args is at the end
        int pos = 0;
        for (auto vd : this->symbol->args) {
            if (vd.name == node->var->name) {
                ZF_ERROR("line %d: duplicate argument %s", node->line, node->var->name.c_str());
            }
            if (vd.type == VA_TYPE) {
                if (pos == 0 || pos != this->symbol->args.size() - 1 || !this->symbol->extc) {
                    ZF_ERROR("line %d: invalid use of varargs", vd.lineno);
                }
            }
            ++pos;
        }
        this->symbol->args.push_back(*node->var);
    }

    bool ret_type_declared; // Whether a return type is declated

    // Now we've passed the function argument list, get return type
    Token cln = lex::lex(file);
    if (cln.type == TreeComp::COLON) {
        ret_type_declared = true;
    } else if (cln.type == TreeComp::OBRACE || cln.type == TreeComp::SEMICOLON) {
        lex::unlex(cln);
        ret_type_declared = false;
    } else {
        ZF_TOK_ERR(cln, "':' or '{'");
    }
    if (ret_type_declared) {
        this->symbol->ret = parse_type(file);
    } else {
        this->symbol->ret = VOID;
    }

    sym::add_global_symbol(this->symbol);

    sym::set_function(this->symbol);

    if (extc) {
        // Expect a semicolon
        auto sc = lex::lex(file);
        if (sc.type != TreeComp::SEMICOLON) {
            ZF_TOK_ERR(sc, ";");
        }
    } else {
        // Now, parse the rest as a block statement
        this->body = new BlockStatementNode();
        this->body->read(file);
    }

    // exit argument scope
    sym::exit_scope();

    sym::set_function(nullptr);

}
