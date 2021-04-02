#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>
#include <back/operator.hpp>

void ExprNode::write(std::ofstream& file) {

    gen::write(file, "(");

    if (this->literal != "") {
        gen::write(file, literal);
        goto out;
    }

    if (this->ref != nullptr) {
        gen::write(file, this->ref->name);
    }

    if (this->left == nullptr)
        ZF_BACK_ERR("expression did not have any content");

    this->left->write(file);
    if (op::opToStr(this->op) != "!!INVALID_OPERATOR!!") {
        gen::write(file, " ");   
        gen::write(file, op::opToStr(this->op));
    }
    if (this->right != nullptr) {
        gen::write(file, " ");
        this->right->write(file);
    }

out:
    gen::write(file, ")");

}
