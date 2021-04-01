/**
 * Compile - provide a function to compile one file to another.
 */

#pragma once

#include <fstream>

#include <ast/ast.hpp>
#include <back/operator.hpp>
#include <util/error.hpp>

void compile(std::string in, std::string out);