#include <ast/ast.hpp>
#include <back/dtype.hpp>
#include <back/gen/gen_main.hpp>

// So we don't use the same counter variable multiple times.
namespace {
    int loopdepth = 0;
}

void LoopNode::write(std::ofstream& file) {

    ++loopdepth;

    Type type = get_type(this->expr);
    if (type == INT) {
        // It's a number. We need to figure out a loop variable.
        std::string begin = "for (int ";
        std::string counterv;
        if (this->pred == nullptr) {
            counterv = "lc_loop_";
            counterv += std::to_string(loopdepth);
        } else if (type == BOOL) {
            counterv = this->pred->var->name;
        } else {
            ZF_ERROR("loop expr must be int or bool: line %d", this->line);
        }
        gen::write(file, begin + counterv + " = 0; " + counterv + " < " + this->expr->literal + "; ++" + counterv + ")");
        this->stmt->write(file);
    } else {
        // Anything else is a boolean. For now.
        gen::write(file, "while (");
        this->expr->write(file);
        gen::write(file, ")");
        this->stmt->write(file);
    }

    --loopdepth;

}