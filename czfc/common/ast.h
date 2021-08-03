/**
 * Abstract syntax tree structures.
 */

#pragma once

#include <util/linkedlist.h>

struct zfa_node;

struct zfa_program {

    /* Just a list of declarations. */
    struct zf_linked_list     decls;

};

/**
 * A single instance of a variable name - a compound name such as x.y.z is only
 * one of these instances.
 */

#define ZF_IDENT_MAXLEN       256

struct zfa_ident {

    /* The name is stored WHOLE here, as "x.y.z" in the case mentioned above -
     * after parsing, the actual link will be resolved.
     */
    char const                namebuf [ ZF_IDENT_MAXLEN ];
    
}
