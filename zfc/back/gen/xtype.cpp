/* Get the type of an expr. As a string for now. */

#include <ast/ast.hpp>
#include <util/error.hpp>

std::string ExprNode::get_type() {
    if (this->literal != "") {
        // It's a literal
        if (this->literal[0] >= '0' && this->literal[0] <= '9') {
            // TODO - floating point numbers
            return "int";
        } else {
            // It's an identifier. TODO - for now we assume int.
            return "int";
        }
    } else {
        // TODO - function calls
        if (this->right == nullptr) {
            ZF_ERROR("symbol tables not yet supported");
        } else {
            if (this->op == "!=" || this->op[0] == '<' || this->op[0] == '>' || this->op == "==") return "bool";
            return this->left->get_type();
        }
    }
}
