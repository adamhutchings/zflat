/**
 * main - main file of the ZFC compiler.
 */

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
        std::cerr << "Cannot open " << argv[1]  << "\n";
        exit(-1);
    }
    Token tok;
    while (lex(file, tok).type != TokenType::TEOF) {
        printf("%-10s   %-10d\n", tok.str.c_str(), tok.type);
    }
    printf("Lexed %d tokens\n", tokens);
}
