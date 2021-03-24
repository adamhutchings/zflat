#include <ast/ast.hpp>
#include <gen_main.hpp>

void ExprNode::write(std::ofstream& file) {

    if (this->literal != "") {
        gen::write(file, literal);
    }

    if (this->left == nullptr || this->right == nullptr || this->op == "")
        ZF_BACK_ERR("expression did not have both left and right");

    this->left->write(file);
    gen::write(file, " ");   
    gen::write(file, this->op);
    gen::write(file, " ");
    this->right->write(file);

}
