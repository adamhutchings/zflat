#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void SwitchNode::write(std::ofstream& file) {

    ControlFlowNode br;
    br.statement = BREAK;
    br.expression = nullptr;
    
    gen::write(file, "switch (");
    this->expr->write(file);
    gen::write(file, ") {");
    gen::indent();
    for (auto cs : this->cases) {
        cs->write(file);
        if (this->fswitch) {
            gen::newline(file);
            br.write(file);
            gen::write(file, ";");
            gen::newline(file);
        }
    }
    gen::unindent();

}
