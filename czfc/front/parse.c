#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <front/lex.h>
#include <output.h>

enum zfp_code {

    ZFPI_GOOD,            /* Nothing wrong */
    ZFPI_SUB,             /* A subroutine of the current routine had an error */
    ZFPI_BUF,             /* Buffer overflow */

};

/* Forward */
static enum zfp_code
zfp_iparse(struct zfa_node * node, struct zf_lexer * lexer);

/**
 * Initialize lexer and after doing validation routines, pass control to the
 * actual core parsing code.
 */
struct zfa_node * zfp_parse(const char * filename) {

    struct zf_lexer           lexer;
    struct zfa_node         * parse_tree;

    parse_tree = NULL;

    if (zf_lexer_init(&lexer, filename)) {
        ZF_PRINT_ERROR("Failed to create lexer.");
        goto error;
    }

    parse_tree = malloc(sizeof *parse_tree);
    if (!parse_tree) {
        ZF_PRINT_ERROR("Failed to allocate parse tree.");
        goto error;
    }
    
    /* Core parse */
    zfp_iparse(parse_tree, &lexer);
    goto out;

error:
    if (parse_tree) {
        free(parse_tree);
        parse_tree = NULL;
    }

out:
    return parse_tree;

}

/* Forward declarations for parse routines */
static enum zfp_code
zfp_parse_program(struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_ident(struct zfa_node * node, struct zf_lexer * lexer);

static enum zfp_code
zfp_iparse(struct zfa_node * node, struct zf_lexer * lexer) {

    return zfp_parse_program(node, lexer);

}

/**
 * Nothing for now.
 */
static enum zfp_code
zfp_parse_program(struct zfa_node * node, struct zf_lexer * lexer) {
    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_PROG;
    return ZFPI_GOOD;
}

static enum zfp_code
zfp_parse_ident(struct zfa_node * node, struct zf_lexer * lexer) {

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_IDENT;

    return ZFPI_GOOD;

}
