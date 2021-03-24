#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void BlockStatementNode::write(std::ofstream& file) {

    gen::writeln(file, " {");
    gen::indent();
    
    for (auto stmt : this->statements)
        stmt->write(file);

    gen::unindent();
    gen::writeln(file, "}");

}
