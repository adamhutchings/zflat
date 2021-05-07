#include <front/parse/parse_header.hpp>

void VarDeclNode::read(std::ifstream& file) {
    Token init = expect(file, IDENTIFIER);
    this->var = new sym::Variable(init.str);
    this->line = init.line;
    expect(file, COLON);
    parse_type(file, &this->var->type);
    auto next = lex::lex(file);
    // followed by anything valid (end of statement)              (end of function arg list) (next argument) opening loop block
    if (next.type == TreeComp::SEMICOLON || next.type == TreeComp::CPAREN || next.type == TreeComp::COMMA || next.type == OBRACE) {
        this->expr = nullptr;
        lex::unlex(next);
    } else if (next.type == TreeComp::OPERATOR) {
        // parse an expression (after an equals sign, check for this later)
        if (op::strToOp(next.str) != op::Operator::EQUALS) {
            ZF_TOK_ERR(next, "'='");
        }
        this->expr = new ExprNode();
        this->expr->read(file);
        if (get_type(this->expr) != this->var->type) {
            ZF_ERROR("line %d: assigning value of type %s to var of type %s"
            , this->line, get_type(this->expr).to_human_str().c_str(), this->var->type.to_human_str().c_str());
        }
    }
    auto* s = sym::resolve_var(this->var->name);
    if (s != nullptr) {
        fprintf(stderr, "zfc: warning: redefining variable \"%s\" on line %d (previous def %d)\n",
        this->var->name.c_str(), this->var->lineno, s->lineno);
    }
    sym::add_symbol(this->var);
}
