#include <ast/ast.hpp>
#include <zfc/back/gen/gen_main.hpp>

void ControlFlowNode::write(std::ofstream& file) {

    gen::write(file, this->statement + " ");
    
    if (this->expression != nullptr)
        this->expression->write(file);

    gen::writeln(file, ";");

}
