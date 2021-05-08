#include <back/gen/gen_main.hpp>
#include <common/operator.hpp>

void BinaryExprNode::write(std::ofstream& file) {
    bool index_expr = this->op == op::Operator::INDEX;
    this->left->write(file);
    if (index_expr) {
        gen::write(file, " [ ");
    } else {
        gen::write(file, op::opToStr(this->op));
    }
    this->right->write(file);
    if (index_expr) {
        gen::write(file, " ] ");
    }
}

void LiteralNode::write(std::ofstream& file) {
    gen::write(file, this->lit.c_str());
}

void VariableNode::write(std::ofstream& file) {
    if (this->ref) gen::write(file, "& ");
    if (this->sym->type->ref) gen::write (file, "* ");
    if (this->sym->type->type_flavor == TT_ENUM) {
        // Generate literal values as literals rather than constants
        auto esym = static_cast<Enum*>(this->sym->type);
        for (auto v : esym->values) {
            auto evsym = reinterpret_cast<sym::EnumVal*>(v);
            if (evsym->name == this->sym->name) {
                gen::write(file, std::to_string(evsym->val));
                break;
            }
        }
    } else {
        gen::write(file, this->sym->name);
    }
}

void ExprNode::write(std::ofstream& file) {

    gen::write(file, "(");

    this->inner->write(file);

    gen::write(file, ")");

}
