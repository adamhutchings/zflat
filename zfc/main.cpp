/**
 * main - main file of the ZFC compiler.
 */

#include <fstream>
#include <iostream>

#include <compile.hpp>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Expected files (for input and output)" << "\n";
        exit(-1);
    }
    compile(argv[1], argv[2]);
}
