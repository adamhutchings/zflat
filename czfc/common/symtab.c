/**
 * Implementations of symbol table functions.
 */

#include "symtab.h"

#include <stdlib.h>

#include <output.h>
#include <util/linkedlist.h>

/**
 * A symbol table scope. Contains sets of definitions, starting and ending lines
 * and the file it's defined in, a parent scope (is NULL for a top-level scope),
 * and a set of child scopes.
 */
struct zf_symscope {

    struct zf_linked_list     defs;
    int                       startline, endline;
    char                    * srcfile;
    struct zf_symscope      * parent;
    struct zf_linked_list     children;

};

/**
 * A full symbol table. Contains only the pointer to the currently active scope,
 * because all sscopes may be accessed through parent / child references.
 */
struct zf_symtab {
    struct zf_symscope * active;
};

void zfscope_init(
    struct zf_symscope * scope,
    struct zf_symscope * parent,
    const char * srcfile,
    int srcline
) {
    scope->parent = parent;
    scope->startline = srcline;
    scope->srcfile = srcfile;
    zfll_init(&scope->defs);
    zfll_init(&scope->children);
}

int zf_symtab_init(struct zf_symtab * symtab) {

    symtab->active = malloc(sizeof *symtab->active);
    if (symtab->active == NULL) {
        ZF_PRINT_ERROR("Failed to allocate symtab root");
        return 1;
    }

    return 0;

}
