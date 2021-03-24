#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void StatementNode::write(std::ofstream& file) {

    this->inner->write(file);
    gen::writeln(file, ";");

}
