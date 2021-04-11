#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void FuncCallNode::write(std::ofstream& file) {

    sym::Function* fnref;
    if ( (fnref = static_cast<sym::Function*>(this->ref)) == nullptr) {
        ZF_BACK_ERR("func call symbol was not a function symbol");
    }
    gen::write(file, fnref->get_overloaded_name());
    gen::write(file, "(");

    for (auto arg : this->args) {
        if (arg != this->args[0])
            gen::write(file, ", ");
        arg->write(file);
    }

    gen::write(file, ")");

}
