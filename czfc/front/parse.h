/**
 * Parse a Z-flat program into a syntax tree.
 */

#pragma once

#include <common/ast.h>

/**
 * Allocate and create a complete syntax tree.
 */
struct zfa_node * zfp_parse(const char * filename);
