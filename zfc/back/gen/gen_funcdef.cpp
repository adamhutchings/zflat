#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>
#include <back/operator.hpp>

void FunctionNode::write(std::ofstream& file) {

    gen::write(file, this->symbol->name == "main" ? "int" : this->symbol->ret.to_output_str());
    gen::write(file, " ");
    gen::write(file, (this->symbol->extc || this->symbol->name == "main") ? this->symbol->name : this->symbol->get_overloaded_name());

    gen::write(file, "(");
    int size = this->symbol->args.size();
    for (int i = 0; i < size; ++i) {
        auto arg = this->symbol->args[i];
        if (i != 0)
            gen::write(file, ",");
        gen::write(file, arg.type.to_output_str() + " ");
        if (arg.name != "$unnamed") {
            gen::write(file, arg.name);
        }
    }
    gen::write(file, ") ");

    if (this->symbol->extc) {
        gen::write(file, ";");
    } else {
        this->body->write(file);
    }

}
