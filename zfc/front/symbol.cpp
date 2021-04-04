#include "symbol.hpp"

namespace {

std::vector<std::vector<sym::Symbol*>> symtab;

sym::Function* func = nullptr;

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

void add_global_symbol(Symbol* s) {
    symtab[0].push_back(s);
}

Symbol* resolve(std::string name) {
    for (int i = symtab.size(); i > 0; --i) {
        for (Symbol* sym : symtab[i - 1]) {
            if (sym->name == name) return sym;
        }
    }
    return nullptr;
}

bool in_global_scope() {
    return symtab.size() == 1;
}

Function* current_function() {
    return func;
}

void set_function(Function* fn) {
    func = fn;
}

} // namespace sym
