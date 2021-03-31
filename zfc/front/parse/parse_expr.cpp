#include <fstream>

#include <ast/ast.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void ExprNode::read(std::ifstream& file) {

    auto start = lex::lex(file);
    
    this->locked = false;

    if (start.type == TreeComp::OPAREN) {
        this->read(file);
        // An extra closing ) should exist.
        auto close = lex::lex(file);
        if (close.type != TreeComp::CPAREN) {
            ZF_TOK_ERR(close, ")");
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
            this->op = next.str;
            this->literal = "";
            this->right = new ExprNode();
            this->right->read(file);
        } else if (next.type == TreeComp::OPAREN) {
            // Put back this and last, parse function call
            lex::unlex(next);
            lex::unlex(start);
            this->left = new FuncCallNode();
            this->left->read(file);
            this->literal = this->op = "";
            this->right = nullptr;
            this->locked = true;
        } else if (next.type == TreeComp::SEMICOLON || next.type == TreeComp::CPAREN || next.type == TreeComp::COMMA) {
            this->literal = start.str;
            this->left = this->right = nullptr;
            this->op = "";
            // Put back the next token, which is not part of the expr.
            lex::unlex(next);
        } else {
            ZF_TOK_ERR(next, "operator, ';', or '('");
        }
    } else {
        // Unary operators - not supported yet
        ZF_TOK_ERR(start, "value");
    }

}
