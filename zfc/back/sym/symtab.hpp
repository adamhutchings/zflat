/**
 * Symtab - utilities for the symbol tables the compiler will construct and
 * analyze, as well as the static list of symbol tables that the compiler
 * will use.
 */

#pragma once

#include <vector>
#include <string>

namespace sym {

struct Symbol {

    std::string name;
    std::string type;

};

struct VariableSymbol : Symbol {
    // nothing needed here yet
};

struct FunctionSymbol : Symbol {
    std::vector<std::string> argtypes;
};

struct SymbolTable {
    std::vector<Symbol*> syms;
    void add(Symbol* sym);
    void pop();
};

void open(SymbolTable* tab);

void close();

extern std::vector<sym::SymbolTable*> tables;

Symbol* lookup(std::string name);

}
