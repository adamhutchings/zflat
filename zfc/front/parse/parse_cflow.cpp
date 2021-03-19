#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

namespace parse {

ControlFlowNode control_flow(std::ifstream& file) {
    ControlFlowNode ret;
    Token tok = lex::lex(file);
    if (tok.type != TreeComp::FLOW)
        ZF_ERROR("expected \"break\", \"continue\", or \"return\" on line %d, instead got \"%u\".", tok.line, tok.type);
    ret.statement = tok.str;
    Token next = lex::lex(file);
    if (next.type == TreeComp::SEMICOLON) {
        ret.expression = NULL;
    } else {
        // TODO: parse expression here
    }
    return ret;
}

}
