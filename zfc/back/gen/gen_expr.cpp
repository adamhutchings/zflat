#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void ExprNode::write(std::ofstream& file) {

    gen::write(file, "(");

    if (this->literal != "") {
        gen::write(file, literal);
        goto out;
    }

    if (this->left == nullptr)
        ZF_BACK_ERR("expression did not have any content");

    this->left->write(file);
    if (this->op != "") {
        gen::write(file, " ");   
        gen::write(file, this->op);
    }
    if (this->right != nullptr) {
        gen::write(file, " ");
        this->right->write(file);
    }

out:
    gen::write(file, ")");

}
