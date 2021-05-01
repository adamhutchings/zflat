/**
 * gen_main - utilities for generating programs.
 */

#pragma once

#include <fstream>
#include <string>
#include <ast/ast.hpp>

#include <util/error.hpp>

#define ZF_BACK_ERR(...) ZF_ERROR("internal backend error: " __VA_ARGS__);

namespace gen {

void write(std::ofstream& file, std::string str);
void newline(std::ofstream& file);

void indent();

void unindent();

}
