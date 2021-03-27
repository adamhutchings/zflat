#include "symtab.hpp"

namespace sym {

std::vector<sym::SymbolTable*> tables;

void SymbolTable::add(Symbol* sym) {
    this->syms.push_back(sym);
}

void SymbolTable::pop() {
    this->syms.pop_back();
}

void open(SymbolTable* tab) {
    tables.push_back(tab);
}

void close() {
    tables.pop_back();
}

}
