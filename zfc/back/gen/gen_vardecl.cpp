#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void VarDeclNode::write(std::ofstream& file) {

    gen::write(file, this->var->type.name);
    gen::write(file, " ");
    gen::write(file, this->var->name);
    
    if (this->expr != nullptr) {
        gen::write(file, " = ");
        this->expr->write(file);
    }

}
