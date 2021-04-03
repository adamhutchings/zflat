#include <fstream>

#include <ast/ast.hpp>
#include <back/dtype.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void FuncCallNode::read(std::ifstream& file) {

    Token name = lex::lex(file);

    if (name.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(name, "identifier");
    } else {
        this->ref = sym::resolve(name.str);
        if (this->ref == nullptr || static_cast<sym::Function*>(this->ref) == nullptr) {
            ZF_ERROR("could not resolve function \"%s\" on line %d", name.raw_content(), name.line);
        }
    }
    Token opn = lex::lex(file);
    if (opn.type != TreeComp::OPAREN) {
        ZF_TOK_ERR(opn, "'('");
    }
    this->line = opn.line;

    Token ctok = lex::lex(file);

    if (ctok.type == TreeComp::CPAREN)
        return; // no arguments

    lex::unlex(ctok);
    auto expected_args = (static_cast<sym::Function*>(this->ref))->args;
    int arg_count = 0, arg_max = expected_args.size();

    while (1) {
        ++arg_count;
        ExprNode* exp = new ExprNode();
        exp->read(file);
        if (get_type(exp) != expected_args[arg_count].type) {
            ZF_ERROR("line %d: expected argument of type %s, found argument of type %s instead"
            , exp->line, typeToStr(expected_args[arg_count].type), typeToStr(get_type(exp)));
        }
        this->args.push_back(exp);
        Token peek = lex::lex(file);
        if (peek.type == TreeComp::COMMA) continue;
        else if (peek.type == TreeComp::CPAREN) goto out;
        else
            ZF_TOK_ERR(peek, "',' or ')'");
    }

out:
    if (arg_count != arg_max) {
        ZF_ERROR("line %d: expected %d arguments to function %s, found %d\n", this->line, arg_max, this->ref->name, arg_max);
    }

}
