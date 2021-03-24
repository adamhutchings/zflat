#include <ast/ast.hpp>
#include <gen_main.hpp>

void VarDeclNode::write(std::ofstream& file) {

    gen::write(file, this->ntype);
    gen::write(file, " ");
    gen::write(file, this->name);
    
    if (this->expr != nullptr) {
        gen::write(file, " = ");
        this->expr->write(file);
    }

}
