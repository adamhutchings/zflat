#include <ast/ast.hpp>
#include <zfc/back/gen/gen_main.hpp>

void StatementNode::write(std::ofstream& file) {

    this->inner->write(file);
    gen::writeln(file, ";");

}
