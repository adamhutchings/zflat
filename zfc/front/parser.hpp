/**
 * Parser - create an abstract syntax tree.
 */

#pragma once

#include <fstream>

#include <ast/ast.hpp>

namespace parse {

ProgramNode parse(std::ifstream& file);

}
