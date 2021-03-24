#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void StatementNode::write(std::ofstream& file) {

    gen::writeln(file, "");
    this->inner->write(file);
    gen::write(file, ";\n");

}
