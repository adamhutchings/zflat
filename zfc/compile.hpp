/**
 * Compile - provide a function to compile one file to another.
 */

#pragma once

#include <fstream>

#include <ast/ast.hpp>
#include <back/reorder.hpp>
#include <util/error.hpp>
#include <util/fs.hpp>

// Only visible for compile individual mode
void compile_unit(std::string path);

void compile_default();
