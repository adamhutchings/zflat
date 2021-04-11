#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void CaseNode::write(std::ofstream& file) {
    gen::write(file, "case ");
    gen::write(file, this->lit);
    gen::write(file, ":");
    gen::indent();
    for (auto st : this->statements) {
        st->write(file);
    }
    gen::unindent();
}
