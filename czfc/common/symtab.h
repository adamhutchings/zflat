/**
 * A complete symbol table for a program.
 */

#pragma once

#include <symbol.h>
#include <util/hashtable.h>
#include <util/linkedlist.h>

struct zf_symtab;

/**
 * Initializes a symtab.
 */
void zf_symtab_init(
    struct zf_symtab * symtab,
    struct zf_symtab * parent,
    const char * srcfile,
    int srcline
);

/**
 * Enter a new scope.
 */
void zfs_enter_scope(struct zf_symtab * symtab);

/**
 * Exit the current scope.
 */
void zfs_exit_scope(struct zf_symtab * symtab);

/**
 * Add a new definition. Returns 1 and prints an error if something happened
 * (e.g. symbol already exists, etc.)
 */
int zfs_add_def(
    struct zf_symtab * symtab,
    const char * name,
    struct zf_symbol * sym
);

/**
 * Get the definition of a symbol. Returns 1 and prints an error if the symbol
 * was not found.
 */
int zfs_get_def(
    struct zf_symtab * symtab,
    const char * name,
    struct zf_symbol * sym
);

/**
 * Destroys a symbol table.
 */
void zfs_destroy(struct zf_symtab * symtab);
