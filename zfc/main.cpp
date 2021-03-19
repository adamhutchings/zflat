/**
 * main - main file of the ZFC compiler.
 */

#include <fstream>
#include <iostream>

#include <front/lex.hpp>

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
    Token tok;
    while ( (tok = lex::lex(file)).type != TreeComp::TEOF ) {
        printf("token: %-10s, type: %-5d\n", tok.str.c_str(), tok.type);
    }
}
