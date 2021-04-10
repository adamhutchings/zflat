/**
 * main - main file of the ZFC compiler.
 */

#include <fstream>
#include <iostream>

#include <compile.hpp>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Expected input file" << "\n";
        exit(-1);
    }
    compile_unit(argv[1]);
}
