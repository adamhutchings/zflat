#include <common/debug.hpp>
#include <back/gen/gen_main.hpp>

void DebugNode::write(std::ofstream& file) {
    
    gen::write(file, "#if ZF_DEBUG_LEVEL >= ");
    gen::write(file, std::to_string(this->debug_val));
    gen::newline(file);

    this->block->write(file);

    gen::newline(file);
    gen::write(file, "#endif /* DEBUG */\n");

}
