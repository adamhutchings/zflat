#include <back/gen/gen_main.hpp>

void ControlFlowNode::write(std::ofstream& file) {

    gen::write(file, flowToStr(this->statement) + " ");
    
    if (this->expression != nullptr)
        this->expression->write(file);

}
