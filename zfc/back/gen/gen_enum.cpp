#include <back/gen/gen_main.hpp>

void EnumDeclNode::write(std::ofstream& file) {

    // Enums are not written - in the output code, all enum constants are
    // replaced with their corresponding literal values.

    // The only thing to write is the typedef.
    std::string def = "typedef ";
    def += typeToCStr(this->sym->underlying_type().primitive);
    def += " ";
    def += this->sym->name;

    gen::write(file, def);

}
