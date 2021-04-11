#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>
#include <back/operator.hpp>

void FunctionNode::write(std::ofstream& file) {

    gen::write(file, typeToStr(this->symbol->ret));
    gen::write(file, " ");
    gen::write(file, this->symbol->get_overloaded_name());

    gen::write(file, "(");
    for (auto arg : this->symbol->args) {
        if (arg.name != this->symbol->args[0].name)
            gen::write(file, ",");
        gen::write(file, typeToStr(arg.type) + " " + arg.name);
    }
    gen::write(file, ") ");

    this->body->write(file);

}
