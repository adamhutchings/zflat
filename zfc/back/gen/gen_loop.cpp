#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

// So we don't use the same counter variable multiple times.
namespace {
    int loopdepth = 0;
}

void LoopNode::write(std::ofstream& file) {

    ++loopdepth;

    // CURSED - figuring out if the expression is a boolean or a number. Someone help!
    if (this->expr->get_type() == "int") {
        // It's a number. We need to figure out a loop variable.
        std::string begin = "for (int ";
        std::string counterv;
        if (this->pred == nullptr) {
            counterv = "lc_loop_";
            counterv += std::to_string(loopdepth);
        } else {
            counterv = this->pred->var->name;
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