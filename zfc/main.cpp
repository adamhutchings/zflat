/**
 * main - main file of the ZFC compiler.
 */

#include <fstream>
#include <iostream>

#include <ast/ast.hpp>
#include <front/parser.hpp>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << "Expected file" << "\n";
        exit(-1);
    }
    std::ifstream file;
    file.open(argv[1]);
    if (!file) {
        std::cerr << "Unable to open " << argv[1] << "\n";
        exit(-1);
    }
    ProgramNode program;
    program.read(file);
}
