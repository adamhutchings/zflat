/**
 * A reference to a variable, a function, or (later) a type.
 */

#pragma once

#include <util/linkedlist.h>

enum zfs_kind {
    ZFS_VARIABLE,
    ZFS_FUNCTION,
    ZFS_MAX,
};

struct zfs_var {

    /* Just the lastmost element. */
    char                    * localname;

    /* TODO - proper type reference */
    char                    * type;

};

struct zfs_func {
    char                    * name;
    char                    * rettype;
    struct zf_linked_list   * paramtypes;
};

struct zfs_sym {

    union {
        struct zfs_var      * var;
        struct zfs_func     * func;
    } as;

    enum zfs_kind             type;

    /* What file this is from, for error messages, etc. */
    char                    * srcfile;
    /* Same deal. */
    int                       srcline, srclinepos;

};
