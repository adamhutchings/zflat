#include <back/gen/gen_main.hpp>

void UseNode::write(std::ofstream& file) {

    for (auto decl : this->symtab) {

        auto* var = static_cast<sym::Variable*>(decl);
        auto* fun = static_cast<sym::Function*>(decl);

        switch (decl->s_type) {
        case sym::SymType::VAR:
            gen::write(file, "extern ");
            gen::write(file, var->type.to_output_str() + " " + var->name + ";");
            return;
        case sym::SymType::FN:
            gen::write(file, fun->ret.to_output_str() + " " + fun->name + "(");
            int ind = 0;
            for (auto arg : fun->args) {
                if (ind != 0) {
                    gen::write(file, ", ");
                }
                gen::write(file, arg.type.to_output_str());
            }
            gen::write(file, ");");
        }

        gen::write(file, "\n");

    }

    gen::write(file, "\n");

}
