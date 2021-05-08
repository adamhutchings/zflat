#include <common/dtype.hpp>
#include <back/gen/gen_main.hpp>

// So we don't use the same counter variable multiple times.
namespace {
    int loopdepth = 0;
}

void LoopNode::write(std::ofstream& file) {

    ++loopdepth;

    Type* type = get_type(this->expr);
    if (*type == INT) {
        // It's a number. We need to figure out a loop variable.
        std::string begin = "for (int ";
        std::string counterv;
        if (this->pred == nullptr) {
            counterv = "lc_loop_";
            counterv += std::to_string(loopdepth);
        } else {
            counterv = this->pred->var->name;
        }
        gen::write(file, begin + counterv + " = 0; " + counterv + " < ");
        this->expr->write(file);
        gen::write(file, std::string("; ++") + counterv + ")");
        this->stmt->write(file);
    } else if (*type == BOOL) {
        // Anything else is a boolean. For now.
        gen::write(file, "while (");
        this->expr->write(file);
        gen::write(file, ")");
        this->stmt->write(file);
    } else {
        ZF_ERROR("loop expr must be int or bool: line %d", this->line);
    }

    --loopdepth;

}