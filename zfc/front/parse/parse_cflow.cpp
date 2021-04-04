#include <fstream>

#include <ast/ast.hpp>
#include <back/dtype.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void ControlFlowNode::read(std::ifstream& file) {
    Token tok = lex::lex(file);
    if (tok.type != TreeComp::FLOW)
        ZF_TOK_ERR(tok, "break, continue, or return");
    this->statement = strToFlow(tok.str);
    this->line = tok.line;
    Token next = lex::lex(file);
    if (sym::in_global_scope()) {
        ZF_ERROR("line %d: control flow in global scope", tok.line);
    }
    if (next.type == TreeComp::SEMICOLON) {
        this->expression = NULL;
        lex::unlex(next); // statement expects a trailing ';'
    } else {
        if (this->statement != RETURN) {
            ZF_ERROR("line %d: did not expect expression after %s", tok.line, tok.raw_content());
        }
        lex::unlex(next);
        this->expression = new ExprNode();
        this->expression->read(file);
    }
    // Check that return type is valid
    if (
        ( (this->expression == nullptr) ? Type::VOID : get_type(this->expression) )
         != sym::current_function()->ret) {
        ZF_ERROR("line %d: expected return type of %s, found expression of type %s"
        , this->expression->line, typeToStr(get_type(this->expression)).c_str(), typeToStr(sym::current_function()->ret).c_str());
    }
}
