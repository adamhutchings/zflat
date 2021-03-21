#include "parser.hpp"

namespace parse {

ProgramNode parse(std::ifstream& file) {
    ProgramNode pnode;
    pnode.read(file);
    return pnode;
}

}
