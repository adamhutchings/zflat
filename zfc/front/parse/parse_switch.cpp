#include <fstream>

#include <ast/ast.hpp>
#include <back/dtype.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void SwitchNode::read(std::ifstream& file) {

    auto sw = lex::lex(file);
    if (sw.type == SWITCH) {
        this->fswitch = false;
    } else if (sw.type == FSWITCH) {
        this->fswitch = true;
    } else {
        ZF_TOK_ERR(sw, "switch or fswitch");
    }

    auto op = lex::lex(file);
    if (op.type != OPAREN) {
        ZF_TOK_ERR(op, "'('");
    }

    this->expr = new ExprNode();
    this->expr->read(file);

    auto cp = lex::lex(file);
    if (cp.type != CPAREN) {
        ZF_TOK_ERR(cp, "')'");
    }

    auto ob = lex::lex(file);
    if (ob.type != OBRACE) {
        ZF_TOK_ERR(ob, "'{'");
    }

    while (1) {
        auto cb = lex::lex(file);
        if (cb.type == OBRACE) {
            break;
        }
        lex::unlex(cb);
        auto cn = new CaseNode();
        cn->read(file);
        this->cases.push_back(cn);
    }

    this->line = sw.line;

}
