#include <back/gen/gen_main.hpp>

void UseNode::write(std::ofstream& file) {

    for (auto decl : this->symtab) {

        auto* var = static_cast<sym::Variable*>(decl);
        auto* fun = static_cast<sym::Function*>(decl);
        int ind;

        switch (decl->s_type) {
        case sym::SymType::VAR:
            gen::write(file, "extern ");
            gen::write(file, var->type.to_output_str() + " " + var->name + ";");
            return;
        case sym::SymType::FN:
            gen::write(file, fun->ret.to_output_str() + " " + fun->name + "(");
            ind = 0;
            for (auto arg : fun->args) {
                if (ind != 0) {
                    gen::write(file, ", ");
                }
                gen::write(file, arg.type.to_output_str());
                ++ind;
            }
            gen::write(file, ");");
        case sym::SymType::T_ENUM:
            // TODO - after refactor to use actual parse/gen
            ;
        case sym::SymType::T_ENUMSYM:
            ;
        }

        gen::write(file, "\n");

    }

    gen::write(file, "\n");

}
