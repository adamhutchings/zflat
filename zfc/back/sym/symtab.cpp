#include "symtab.hpp"

namespace {

std::vector<sym::SymbolTable*> tables;

}

namespace sym {

void open(SymbolTable* tab) {
    tables.push_back(tab);
}

void close() {
    tables.pop_back();
}

}
