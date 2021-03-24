#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void BlockStatementNode::write(std::ofstream& file) {

    gen::write(file," {");
    gen::indent();
    gen::writeln(file, "");
    
    for (auto stmt : this->statements)
        stmt->write(file);

    gen::unindent();
    gen::writeln(file, "}\n");

}
