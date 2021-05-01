#include <back/gen/gen_main.hpp>

void IfNode::write(std::ofstream& file) {

    gen::write(file, "if (");
    this->expr->write(file);
    gen::write(file, ") ");
    this->stmt->write(file);
    if (this->else_block != nullptr) {
        gen::write(file, " else");
        this->else_block->write(file);
    }

}
