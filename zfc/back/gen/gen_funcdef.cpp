#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void FunctionNode::write(std::ofstream& file) {

    gen::write(file, this->symbol->ret.name);
    gen::write(file, " ");
    gen::write(file, this->symbol->name);

    gen::write(file, "(");
    for (auto arg : this->symbol->args) {
        if (arg.name != this->symbol->args[0].name)
            gen::write(file, ",");
        gen::write(file, arg.type.name + " " + arg.name);
    }
    gen::write(file, ") ");

    this->body->write(file);

}
