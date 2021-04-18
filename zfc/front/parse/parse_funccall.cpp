#include <fstream>

#include <ast/ast.hpp>
#include <back/dtype.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void FuncCallNode::read(std::ifstream& file) {

    Token name = lex::lex(file);
    if (name.type != TreeComp::IDENTIFIER) {
        ZF_TOK_ERR(name, "identifier");
    }

    Token opn = lex::lex(file);
    if (opn.type != TreeComp::OPAREN) {
        ZF_TOK_ERR(opn, "'('");
    }
    this->line = opn.line;

    Token ctok = lex::lex(file);

    if (ctok.type == TreeComp::CPAREN)
        goto out; // no arguments

    lex::unlex(ctok);

    while (1) {
        ExprNode* exp = new ExprNode();
        exp->read(file);
        this->args.push_back(exp);
        Token peek = lex::lex(file);
        if (peek.type == TreeComp::COMMA) continue;
        else if (peek.type == TreeComp::CPAREN) goto out;
        else
            ZF_TOK_ERR(peek, "',' or ')'");
    }

out:

    std::vector<Type> args;
    for (auto var : this->args) {
        args.push_back(get_type(var));
    }

    this->call = sym::resolve_fn(name.str, args);
    if (this->call == nullptr) {
        ZF_ERROR("could not resolve function \"%s\" on line %d", name.raw_content(), name.line);
    }

    // For argument verification
    bool varargs = this->call->args[this->call->args.size() - 1].type == VA_TYPE;

    auto expected_args = this->call->args;
    int arg_max = expected_args.size();
    int actual_args = this->args.size();

    bool arg_count_mismatch =
        (varargs) ? actual_args < arg_max - 1 : actual_args != arg_max;

    if (arg_count_mismatch) {
        ZF_ERROR("line %d: expected %lu arguments to function %s, found %lu\n", this->line, this->args.size(), this->call->name.c_str(), this->args.size());
    }

    int arg_id = 0;
    for (auto exp : this->args) {
        // Only check arguments that aren't varargs
        if (expected_args[arg_id].type == VA_TYPE) break;
        if (get_type(exp) != expected_args[arg_id].type) {
            ZF_ERROR("line %d: expected argument of type %s, found argument of type %s instead"
            , exp->line, expected_args[arg_id].type.to_human_str().c_str(), get_type(exp).to_human_str().c_str());
        }
        ++arg_id;
    }

}
