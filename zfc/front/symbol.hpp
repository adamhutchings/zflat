/**
 * symbol - symbols that AST nodes will have references to.
 */

#pragma once

#include <string>
#include <vector>

#include <back/type.hpp>

namespace sym {

struct Symbol {
    std::string name;
    int lineno;
    inline Symbol(std::string n) : name(n) {}
};

struct Variable : public Symbol {
    Type type;
    inline Variable(std::string n) : Symbol(n) {}
};

struct Function : public Symbol {
    std::vector<Variable> args;
    Type ret;
    inline Function(std::string n) : Symbol(n) {}
};

void enter_scope();
void exit_scope();

void add_symbol(Symbol* s);

Symbol* resolve(std::string name);

}
