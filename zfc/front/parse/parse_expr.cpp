#include <fstream>

#include <ast/ast.hpp>
#include <back/operator.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

void ExprNode::read(std::ifstream& file) {

    auto start = lex::lex(file);
    
    this->locked = false;
    this->ref = nullptr;

    if (start.type == TreeComp::OPAREN) {
        this->locked = true;
        this->read(file);
        // An extra closing ) should exist.
        auto close = lex::lex(file);
        if (close.type != TreeComp::CPAREN) {
            ZF_TOK_ERR(close, ")");
        }
        auto next = lex::lex(file);
        if (next.type == TreeComp::OPERATOR) {
            // Assign "this" to this->left
            // Copy the left node
            ExprNode* left = this->left;
            // Copy this to this->left
            this->left = new ExprNode();
            this->left->right = this->right;
            this->left->left = left;
            this->left->op = this->op;
            this->left->ref = this->ref;
            // Make a new right node
            this->op = op::strToOp(next.str);
            this->right = new ExprNode();
            this->right->read(file);
        } else {
            lex::unlex(next);
        }
    } else if (start.type == TreeComp::LITERAL || start.type == TreeComp::IDENTIFIER) {
        // We expect an operator or a function call.
        auto next = lex::lex(file);
        if (next.type == TreeComp::OPERATOR) {
            this->line = next.line;
            ExprNode* left_expr = new ExprNode();
            if (start.type == TreeComp::LITERAL) {
                left_expr->literal = start.str;
                left_expr->ref = nullptr;
            } else {
                if (start.str[0] >= '0' && start.str[0] >= '9') {
                    left_expr->literal = start.str;
                } else {
                    left_expr->ref = sym::resolve(start.str);
                    if (left_expr->ref == nullptr) {
                        ZF_ERROR("could not resolve variable \"%s\" on line %d", start.raw_content(), start.line);
                    }
                    left_expr->literal = "";
                }
            }
            left_expr->left = left_expr->right = nullptr;
            left_expr->op = op::Operator::INVALID;
            this->left = left_expr;
            this->op = op::strToOp(next.str);
            this->literal = "";
            this->ref = nullptr;
            this->right = new ExprNode();
            this->right->read(file);
        } else if (next.type == TreeComp::OPAREN) {
            // Put back this and last, parse function call
            lex::unlex(next);
            lex::unlex(start);
            this->left = new FuncCallNode();
            this->left->read(file);
            this->literal = "";
            this->op = op::Operator::INVALID;
            this->right = nullptr;
            this->locked = true;
            this->line = this->left->line;
        } else if (next.type == TreeComp::SEMICOLON || next.type == TreeComp::CPAREN || next.type == TreeComp::COMMA) {
            this->literal = start.str;
            this->left = this->right = nullptr;
            this->op = op::Operator::INVALID;
            // Put back the next token, which is not part of the expr.
            lex::unlex(next);
            this->line = start.line;
        } else {
            ZF_TOK_ERR(next, "operator, ';', or '('");
        }
    } else {
        // Unary operators - not supported yet
        ZF_TOK_ERR(start, "value");
    }

    // Check to make sure left-hand side is an lvalue
    if (this->op != op::Operator::INVALID && op::is_assign(this->op)) {
        if (this->left->ref == nullptr) {
            ZF_ERROR("line %d: invalid lvalue (expected identifier)", this->left->line);
        }
    }

}
