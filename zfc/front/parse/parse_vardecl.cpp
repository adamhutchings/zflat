#include <fstream>

#include <ast/ast.hpp>
#include <back/dtype.hpp>
#include <back/operator.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void VarDeclNode::read(std::ifstream& file) {
    Token init = lex::lex(file);
    if (init.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(init, "identifier");
    } else {
        this->var = new sym::Variable(init.str);
        this->line = init.line;
    }
    Token next = lex::lex(file);
    if (next.type != TreeComp::COLON) {
        ZF_TOK_ERR(next, "':'");
    }
    next = lex::lex(file);
    if (next.type != TreeComp::TYPENAME) {
        ZF_TOK_ERR(next, "type name");
    } else {
        this->var->type = strToType(next.str);
    }
    next = lex::lex(file);
    // followed by anything valid (end of statement)              (end of function arg list) (next argument) opening loop block
    if (next.type == TreeComp::SEMICOLON || next.type == TreeComp::CPAREN || next.type == TreeComp::COMMA || next.type == OBRACE) {
        this->expr = nullptr;
        lex::unlex(next);
    } else if (next.type == TreeComp::OPERATOR) {
        // parse an expression (after an equals sign, check for this later)
        this->expr = new ExprNode();
        this->expr->read(file);
        if (get_type(this->expr) != this->var->type) {
            ZF_ERROR("line %d: assigning value of type %s to var of type %s"
            , this->line, typeToStr(get_type(this->expr)).c_str(), typeToStr(this->var->type).c_str());
        }
    }
    auto* s = sym::resolve_var(this->var->name);
    if (s != nullptr) {
        fprintf(stderr, "zfc: warning: redefining variable \"%s\" on line %d (previous def %d)\n",
        this->var->name.c_str(), this->var->lineno, s->lineno);
    }
    sym::add_symbol(this->var);
}
