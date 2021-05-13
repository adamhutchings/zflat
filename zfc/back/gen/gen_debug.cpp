#include <common/debug.hpp>
#include <back/gen/gen_main.hpp>

void DebugNode::write(std::ofstream& file) {
    
    gen::write(file, "#if ZF_DEBUG_LEVEL >= ");
    gen::write(file, std::to_string(this->debug_val));
    gen::write(file, "\n");

    this->block->write(file);

    gen::write(file, "\n#endif /* DEBUG */\n");

}
