#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>
#include <back/operator.hpp>

void FunctionNode::write(std::ofstream& file) {

    gen::write(file, this->symbol->ret.to_output_str());
    gen::write(file, " ");
    gen::write(file, this->symbol->extc ? this->symbol->name : this->symbol->get_overloaded_name());

    gen::write(file, "(");
    for (auto arg : this->symbol->args) {
        if (arg.name != this->symbol->args[0].name)
            gen::write(file, ",");
        gen::write(file, arg.type.to_output_str() + " " + arg.name);
    }
    gen::write(file, ") ");

    if (this->symbol->extc) {
        gen::write(file, ";");
    } else {
        this->body->write(file);
    }

}
