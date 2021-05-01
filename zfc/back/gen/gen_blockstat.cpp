#include <back/gen/gen_main.hpp>

void BlockStatementNode::write(std::ofstream& file) {

    gen::write(file, "{");
    gen::indent();
    
    for (auto stmt : this->statements)
        stmt->write(file);

    gen::unindent();
    gen::newline(file);
    gen::write(file, "}");

}
