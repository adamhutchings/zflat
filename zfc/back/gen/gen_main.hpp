/**
 * gen_main - utilities for generating programs.
 */

#pragma once

#include <fstream>
#include <string>

#include <util/error.hpp>

#define ZF_BACK_ERR(msg, ...) ZF_ERROR("internal backend error: " __VA_ARGS__);

namespace gen {

void writeln(std::ofstream& file, std::string str);
void write(std::ofstream& file, std::string str);

void indent();

void unindent();

}
