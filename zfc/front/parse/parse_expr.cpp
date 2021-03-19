#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void ExprNode::read(std::ifstream& file) {

    auto start = lex::lex(file);

    if (start.type == TreeComp::OPAREN) {
        this->read(file);
        // An extra closing ) should exist.
        auto close = lex::lex(file);
        if (close.type != TreeComp::CPAREN) {
            ZF_ERROR("expected ) on line %u, found \"%s\" instead.", close.line, close.raw_content());
        }
    } else if (start.type == TreeComp::LITERAL || start.type == TreeComp::IDENTIFIER) {
        // We expect an operator or a function call.
        auto next = lex::lex(file);
        if (next.type == TreeComp::OPERATOR) {
            ExprNode* left_expr = new ExprNode();
            left_expr->literal = start.str;
            left_expr->left = left_expr->right = nullptr;
            left_expr->op = "";
            this->left = left_expr;
        } else if (next.type == TreeComp::OPAREN) {
            // TODO - function call
        } else if (next.type == TreeComp::SEMICOLON || next.type == TreeComp::CPAREN) {
            this->literal = next.str;
            this->left = this->right = nullptr;
            this->op = "";
            // Put back the next token, which is not part of the expr.
            lex::unlex(next);
        } else {
            ZF_ERROR("expected an operator, ';' or '(' after %s on line %u, found %s instead.", start.raw_content(), start.line, next.raw_content());
        }
    } else {
        // Unary operators - not supported yet
        ZF_ERROR("expected a value on line %u, found \"%s\" instead", start.line, start.str.c_str());
    }

}
