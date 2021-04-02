#include "symbol.hpp"

namespace {

std::vector<std::vector<sym::Symbol*>> symtab;

} // namespace

namespace sym {

void enter_scope() {
    symtab.push_back(std::vector<Symbol*>());
}
void exit_scope() {
    symtab.pop_back();
}

void add_symbol(Symbol* s) {
    symtab[symtab.size() - 1].push_back(s);
}

Symbol* resolve(std::string name) {
    for (int i = symtab.size(); i > 0; --i) {
        for (Symbol* sym : symtab[i - 1]) {
            if (sym->name == name) return sym;
        }
    }
    return nullptr;
}

} // namespace sym
