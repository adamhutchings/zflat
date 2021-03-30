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

Symbol* lookup(std::string name) {
    SymbolTable* tab;
    int i = tables.size();
    while (i) {
        tab = tables[i - 1];
        for (auto scopesymbol : tab->syms) {
            if (scopesymbol->name == name)
                return scopesymbol;
        }
    }
}

}
