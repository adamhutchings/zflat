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

    struct zf_hashtable       defs;
    int                       startline, endline;
    const char              * srcfile;
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

int zfscope_init(
    struct zf_symscope * scope,
    struct zf_symscope * parent,
    const char * srcfile,
    int srcline
) {
    scope->parent = parent;
    scope->startline = srcline;
    scope->srcfile = srcfile;
    zf_hashtable_init(&scope->defs);
    zfll_init(&scope->children);
    return 0;
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

/**
 * Get the location of a symbol without doing any erroring itself. Returns a
 * pointer to the location, and stuffs depth with how many scope levels deep
 * the definition is if found (or -1 if no def), where 0 is the current scope,
 * 1 is the scope outside this one, etc.
 */
static struct zfs_sym * zfs_locate_def(
    struct zf_symtab * symtab,
    const char * name,
    int * level
) {

    /* Scope being searched. */
    struct zf_symscope * scope;

    /* Start at -1 for no def found. */
    *level = -1;
    scope = symtab->active;
    void * symbol;
    symbol = NULL;

    do {

        /* Increment at the beginning of the loop, so if a def is found right
         * away, get level set to 0.
         */
        ++*level;

        if (scope == NULL) break;
        symbol = zf_hashtable_get(&scope->defs, name);
        if (symbol) {
            break; /* Found a def */
        }

        /* Search through next scope */
        scope = scope->parent;

    } while (1);

    return symbol;

}

int zfs_add_def(
    struct zf_symtab * symtab, const char * name, struct zfs_sym * sym
) {

    /* The location to add. */
    struct zfs_sym * symbol;
    int depth;

    symbol = zfs_locate_def(symtab, name, &depth);
    if (symbol) {
        /* Check depth. If redeclaring outside of scope, warn - if redeclaring
         * in scope, error.
         */
        if (depth == 0) {
            ZF_PRINT_ERROR("Redeclaring symbol '%s' in current scope", name);
        } else {
            ZF_PRINT_WARNING("Redeclaring symbol '%s'", name);
        }
        ZF_PRINT_WARNING(
            "Note: previous declaration file %s, line %d",
            symbol->srcfile,
            symbol->srcline
        );
        if (depth == 0) {
            return 1; /* Error */
        }
    }

    /* Add the symbol to the current scope. */
    zf_hashtable_set(&symtab->active->defs, name, sym);
    return 0;

}
