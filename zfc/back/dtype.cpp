#include "dtype.hpp"

Type get_type(ExprNode* expr) {
    if (expr->left != nullptr && expr->right == nullptr) {
        return static_cast<sym::Function*>(dynamic_cast<FuncCallNode*>(expr->left)->ref)->ret;
    }
    if (expr->left != nullptr && expr->right != nullptr) {

        Type left = get_type(expr->left), right = get_type(expr->right);
        if (left == VOID || right == VOID) {
            ZF_ERROR("invalid expression on line %d: operand cannot have void type", expr->line);
        }

        if (left != right) {
            ZF_ERROR("line %d: operands don't match types", expr->line);
        }

        // Check ==, !=, etc., and <, >, etc.
        if (
            ((expr->op / OP_GROUP_SIZE) == 1)
        ||  ((expr->op / OP_GROUP_SIZE) == 2)
        ) {
            return BOOL;
        }

        return left;

    }
    // TODO
    return INT; // the only literal type
}
