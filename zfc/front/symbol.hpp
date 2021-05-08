/**
 * symbol - symbols that AST nodes will have references to.
 */

#pragma once

#include <string>
#include <vector>

#include <common/type.hpp>

namespace sym {

extern std::string argsep, retsep, scopesep;

enum SymType {
    VAR,
    FN,
    T_ENUM,
    T_ENUMSYM,
};

struct Symbol {
    std::string name;
    int lineno;
    SymType s_type;
    inline Symbol(std::string n, SymType s) : name(n), s_type(s) {}
    inline bool is_type() { return s_type == T_ENUM /* || other types (T_CLASS, etc.) */ ; }
};

struct Variable : public Symbol {
    Type* type;
    inline Variable(std::string n) : Symbol(n, VAR) {}
};

struct Function : public Symbol {
    std::vector<Variable> args;
    Type ret;
    bool extc;
    inline Function(std::string n) : Symbol(n, FN) {}
    std::string get_overloaded_name();
};

struct EnumVal : public Variable {
    int val;
    Enum* parent;
    inline EnumVal(std::string n, int v, Enum* p) : Variable(n), parent(p) { val = v; s_type = T_ENUMSYM; }
};

void enter_scope();
void exit_scope();

void add_symbol(Symbol* s);
void add_global_symbol(Symbol* s);

Variable* resolve_var(std::string name);
Function* resolve_fn(std::string name, std::vector<Type> args);

bool in_global_scope();

Function* current_function();
void set_function(Function* fn);

std::vector<std::vector<sym::Symbol*>>* getsymtab();

}
