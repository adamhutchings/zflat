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
    char                      namebuf [ ZF_IDENT_MAXLEN ];
    int                       namebuf_len;

};

/* A value which is not a literal. */

#define ZF_VALUE_MAXLEN       256

struct zfa_value {
    char                      namebuf [ ZF_VALUE_MAXLEN ];
    int                       namebuf_len;
};

#define OPBUF_SIZE            8

/**
 * An expression has a left-hand side, an operator, and a right-hand side. Both
 * sides may be either a value or another expression.
 */
struct zfa_expr {

    struct zfa_node         * left, * right;
    /* We'll make this better in the future. */
    char                      opbuf [ OPBUF_SIZE ];

};

#define TYPE_MAX_LEN          64

struct zfa_decl {

    char                      typebuf [ TYPE_MAX_LEN ];
    int                       typebuf_len;

    struct zfa_node         * expr; /* Can be NULL, as in int x; */
    /* Or not, as in int x = 2; */

    char                      namebuf [ ZF_IDENT_MAXLEN ];
    int                       namebuf_len;

};

struct zfa_function {

    /* A complete function declaration */
    struct zfa_decl           decl; /* expr here must be NULL */

    /* exprs on these must be null for now, but not when we make default args */
    struct zf_linked_list     params; /* linked list of decl */

    struct zfa_node         * body;

};

struct zfa_block_stmt {

    /* A list of statement pointers - a statement of type expr is a statement
    which is just like x = 2, a statement of type funccall is a statement which
    is like x(2), etc. */
    struct zf_linked_list     stmts;

};

struct zfa_funccall {
    struct zfa_node         * ident;
    struct zf_linked_list     params;
};

struct zfa_node {

    /* The type of the node. */
    enum zfa_node_type {
        ZFA_NODE_NONE,        /* For when a node is NULL */
        ZFA_NODE_PROG,        /* A program */
        ZFA_NODE_IDENT,       /* A declaration */
        ZFA_NODE_VALUE,       /* A value */
        ZFA_NODE_EXPR,        /* An expression */
        ZFA_NODE_DECL,        /* A declaration */
        ZFA_NODE_FUNCCALL,    /* A function call */
        ZFA_NODE_BLOCK_STMT,  /* A block statement */
        ZFA_NODE_FUNCTION,    /* A function declaration */
    } type;

    union {
        struct zfa_program     prog;
        struct zfa_ident       ident;
        struct zfa_value       value;
        struct zfa_expr        expr;
        struct zfa_decl        decl;
        struct zfa_funccall    funccall;
        struct zfa_block_stmt  blockstmt;
        struct zfa_function    function;
    } as; /* Named "as" so we can say node "as" ident, etc. */

};
