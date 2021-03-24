#include <ast/ast.hpp>
#include <gen_main.hpp>

void FunctionNode::write(std::ofstream& file) {

    gen::write(file, this->ret_type);
    gen::write(file, " ");
    gen::write(file, this->name);

    gen::write(file, "(");
    for (auto arg : this->args) {
        arg->write(file);
        gen::write(file, ", ");
    }
    gen::write(file, ")");

    this->body->write(file);

}
