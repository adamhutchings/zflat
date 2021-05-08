/**
 * Determine type.
 */

#pragma once

#include <common/ast.hpp>
#include <common/type.hpp>

Type* get_type(ExprNode* expr);

// Whether they can appear in a binary expr together
bool are_types_compatible(Type*, Type*);
