#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void FuncCallNode::write(std::ofstream& file) {

    gen::write(file, this->name);
    gen::write(file, "(");

    for (auto arg : this->args) {
        arg->write(file);
        gen::write(file, ",");
    }

    gen::write(file, ")");

}
