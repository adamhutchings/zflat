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

int zfs_enter_scope(struct zf_symtab * symtab, int line) {

    /* Initialize a new scope with the parent of the current scope and init it
     * and everything. */
    struct zf_symscope * new;
    new = malloc(sizeof *new);
    if (!new) {
        ZF_PRINT_ERROR("Cannot allocate new scope");
        return 1;
    }

    zfscope_init(new, symtab->active, symtab->active->srcfile, line);
    zfll_add(&symtab->active->children, new);
    
    return 0;

}

int zfs_exit_scope(struct zf_symtab * symtab, int line) {

    symtab->active->endline = line;
    symtab->active = symtab->active->parent;
    return 0;

}
