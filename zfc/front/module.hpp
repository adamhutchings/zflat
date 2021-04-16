/**
 * module - provides facilities for reading and writing symbol files.
 */

#pragma once

#include <front/symbol.hpp>

namespace mod {

sym::Symbol* readsym(std::ifstream& file);
void writesym(std::ofstream& file, sym::Symbol* sym);

void begin_read(std::ifstream& file);
void end_read(std::ifstream& file);

void begin_write(std::ifstream& file);
void end_write(std::ifstream& file);

}
