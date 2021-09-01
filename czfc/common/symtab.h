/**
 * A complete symbol table for a program.
 */

#pragma once

#include <common/symbol.h>
#include <util/hashtable.h>
#include <util/linkedlist.h>

struct zf_symtab;

/**
 * Initializes a symtab. Returns 1 if error.
 */
int zf_symtab_init(struct zf_symtab * symtab);

/**
 * Enter a new scope at a given line.
 */
int zfs_enter_scope(struct zf_symtab * symtab, int line);

/**
 * Exit the current scope at a given line.
 */
int zfs_exit_scope(struct zf_symtab * symtab, int line);

/**
 * Add a new definition. Returns 1 and prints an error if something happened
 * (e.g. symbol already exists, etc.)
 */
int zfs_add_def(
    struct zf_symtab * symtab,
    const char * name,
    struct zfs_sym * sym
);

/**
 * Get the definition of a symbol. Returns 1 and prints an error if the symbol
 * was not found.
 */
int zfs_get_def(
    struct zf_symtab * symtab,
    const char * name,
    struct zfs_sym * sym
);

/**
 * Destroys a symbol table.
 */
void zfs_destroy(struct zf_symtab * symtab);
