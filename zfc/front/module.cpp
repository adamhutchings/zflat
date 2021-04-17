#include "module.hpp"

/**
 * SYMBOL FILE FORMAT
 */

/**
 * version x.y.z
 * factorial_i(int ): int
 * 
 * end
 */

#define ZF_SYMBOL_ERR(...) ZF_ERROR("symbol: " __VA_ARGS__)

#define MIN(x, y) ( (x) < (y) ? (x) : (y) )

namespace {

Type get_type(std::string str) {

}

sym::Symbol* parse_sym(std::string str) {
    // Find initial symbol (up to paren or colon)
    int sp = str.find(' ');
    int op = str.find('(');
    int symend = MIN(sp, op);
    if (symend < 0) {
        ZF_SYMBOL_ERR("internal: no space in line");
    }
    auto symname = str.substr(0, symend);
    if (symend == op) {
        // This is a function def
        auto ret = new sym::Function(symname);
        str = str.substr(op + 1);
        while (1) {
            if (str[0] == ')') {
                // End of fndef.
                str = str.substr(1);
                break;
            }
            // Parse next arg.
            int comma = str.find(','), cparen = str.find(')'), end = MIN(comma, cparen);
            if (comma < 0) {
                ZF_SYMBOL_ERR("internal: no ',' or ')' found");
            }
            auto next_argtype = str.substr(0, comma);
            auto var = sym::Variable("outside-module-invisible");
            var.type = get_type(next_argtype);
            ret->args.push_back(var);
            // Skip forward
            if (str[0] != ')') {
                ZF_SYMBOL_ERR("internal: expected space after argument");
            }
            str = str.substr(1);
        }
        // Parse return type
        if (str[0] == ';') {
            ret->ret = VOID;
        } else {
            // Expect colon
            if (str[0] != ':') {
                ZF_SYMBOL_ERR("internal: expected colon - return type");
            }
            str = str.substr(1);
            // The rest should be a string of the type name
            ret->ret = get_type(str);
        }
        return ret;
    } else {
        // Parse variable type - this should be a bit easier than the function
        // This is suspiciously similar to code above. Combine?
        auto ret = new sym::Variable(symname);
        if (str[0] != ':') {
            ZF_SYMBOL_ERR("internal: expected colon - return type");
        }
        str = str.substr(1);
        // The rest should be a string of the type name
        ret->type = get_type(str);
        return ret;
    }
}

}

namespace mod {

void begin_read(std::ifstream& file) {
    std::string version_header;
    getline(file, version_header);
    if (version_header.find("version") != 0) {
        // ERROR??
    }
}

sym::Symbol* readsym(std::ifstream& file) {
    std::string symstr;
    getline(file, symstr);
    if (symstr == "") return nullptr;
    return parse_sym(symstr);
}

void end_read(std::ifstream& file) {
    std::string endmark;
    getline(file, endmark);
    if (endmark != "end") {
        // ERROR??
    }
}

void begin_write(std::ofstream& file) {
    file << "version 0.0.1\n";
}

void writesym(std::ofstream& file, sym::Symbol* sym) {
    sym::Variable* var = static_cast<sym::Variable*>(sym);
    if (var != nullptr) {
        // Write x: int, for example
        file << var->name << ": " << var->type.to_str() << "\n";
        return;
    }
    sym::Function* fun = static_cast<sym::Function*>(sym);
    if (fun != nullptr) {
        if (fun->extc) {
            file << "extc ";
        }
        file << fun->name << " (";
        int ct = 0;
        for (auto arg : fun->args) {
            if (ct != 0) {
                file << ", ";
            }
            file << arg.type.to_str();
            ++ct;
        }
        file << "): ";
        file << fun->ret.to_str();
        file << "\n";
    }
}

void end_write(std::ofstream& file) {
    file << "end\n";
}

}
