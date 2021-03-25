/**
 * Compile - provide a function to compile one file to another.
 */

#pragma once

#include <fstream>

#include <ast/ast.hpp>
#include <util/error.hpp>
#include <util/fs.hpp>

void compile(std::string in, std::string out);