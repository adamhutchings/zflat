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
    ZFPI_ALLOC,           /* Memory allocation error */
    ZFPI_TOK,             /* Unexpected token */

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
    if (zfp_iparse(parse_tree, &lexer)) {
        ZF_PRINT_ERROR("parse failed");
    }
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
zfp_parse_ident(struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_value(struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_expr(struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_decl(struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_funccall(struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_blockstmt(struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_function(struct zfa_node * node, struct zf_lexer * lexer);

static enum zfp_code
zfp_iparse(struct zfa_node * node, struct zf_lexer * lexer) {

    return zfp_parse_program(node, lexer);

}

/**
 * Print diagnostic for token errors.
 */
#define ZFP_TOKEN_ERROR(lexer, expected, token) \
    ZF_PRINT_ERROR(                             \
        "line %d: expected %s, found token \"%s\"", \
        lexer->lineno, expected, token.data \
    )

/**
 * Parse a list of function and variable declarations.
 */
static enum zfp_code
zfp_parse_program(struct zfa_node * node, struct zf_lexer * lexer) {

    struct zf_token test1, test2;
    struct zfa_node * decl;

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_PROG;

    /* Start the list of decls */
    zfll_init(&node->as.prog.decls);

    for (;;) {

        zf_lex(lexer, &test1);
        if (test1.type == ZFT_EOF) {
            /* We're done parsing */
            break;
        }
        zf_lex(lexer, &test2);

        /**
         * A variable declaration begins with name - colon (x : int), and a
         * function declaration begins with name - oparen (x (): int).
         */
        if (test1.type != ZFT_IDENT) {
            ZFP_TOKEN_ERROR(lexer, "identifier", test1);
            return ZFPI_TOK;
        }

        /* Allocate more space for a decl */
        if ( !(decl = malloc(sizeof *decl)) ) {
            ZF_PRINT_ERROR("Failed to allocate declaration node.");
            return ZFPI_ALLOC;
        }

        /* Unlex the tokens. If someday the unlex function modifies the tokens,
         * PLEASE FIX HERE!
         */
        zf_unlex(lexer, &test2);
        zf_unlex(lexer, &test1);

        switch (test2.type) {
        case ZFT_COLON:
            /* Variable declaration */
            if (zfp_parse_decl(decl, lexer)) {
                return ZFPI_SUB;
            }
            break;
        case ZFT_OPAREN:
            /* Function declaration */
            if (zfp_parse_function(decl, lexer)) {
                return ZFPI_SUB;
            }
            break;
        default:
            ZFP_TOKEN_ERROR(lexer, ": or (", test2);
            return ZFPI_TOK;
        }

        /* Add the new decl to the list */
        zfll_add(&node->as.prog.decls, decl);

    }

    return ZFPI_GOOD;

}

static enum zfp_code
zfp_parse_ident(struct zfa_node * node, struct zf_lexer * lexer) {

    struct zf_token           token;

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_IDENT;

    for(;;) {

        zf_lex(lexer, &token);
        if (token.type != ZFT_IDENT) {
            ZFP_TOKEN_ERROR(lexer, "identifier", token);
            return ZFPI_TOK;
        }

        /* Add token contents to buffer. */
        
        /* But first, check for buffer overflow. */
        /* nul terminator is already in buffer, don't check */
        if (
            node->as.ident.namebuf_len + strlen(token.data)
            > ZF_IDENT_MAXLEN) {
            goto buf;
        }

        strcpy(node->as.ident.namebuf + node->as.ident.namebuf_len, token.data);

        /* Because names can be x.y.z, check for dot - if dot, add to buffer and
         * continue on. Otherwise, unlex non-dot token. */
        zf_lex(lexer, &token);
        if (token.type != ZFT_DOT) {
            zf_unlex(lexer, &token);
            break;
        } else {
            /* one extra byte for dot */
            if (node->as.ident.namebuf_len + 1 > ZF_IDENT_MAXLEN) {
                goto buf;
            }
            node->as.ident.namebuf[node->as.ident.namebuf_len++] = '.';
        }

    }

    return ZFPI_GOOD;

buf:
    ZF_PRINT_ERROR("line %d: identifier too long", lexer->lineno);
    return ZFPI_BUF;

}

static enum zfp_code
zfp_parse_value(struct zfa_node * node, struct zf_lexer * lexer) {
    
    struct zf_token           token;

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_VALUE;

    zf_lex(lexer, &token);

    if (token.type != ZFT_LITERAL) {
        ZFP_TOKEN_ERROR(lexer, "literal", token);
        return ZFPI_TOK;
    }

    if (token.len > ZF_VALUE_MAXLEN - 1) {
        ZF_PRINT_ERROR("line %d: literal too long", lexer->lineno);
        return ZFPI_BUF;
    }

    strcpy(node->as.value.namebuf, token.data);
    node->as.value.namebuf_len = token.len;

    return ZFPI_GOOD;

}

static enum zfp_code
zfp_parse_expr(struct zfa_node * node, struct zf_lexer * lexer) {

    struct zf_token           token;
    /* This can be used as an intermediate value */
    struct zfa_node         * expr;
    enum zfp_code             code;
    
    /* TODO - operator precedence. Oh boy. */

    zf_lex(lexer, &token);

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_EXPR;

    /* In case we need to stuff this with a value */
    if ( !(expr = malloc(sizeof *expr)) ) {
        ZF_PRINT_ERROR("Failed to allocate expression node.");
        return ZFPI_ALLOC;
    }

    /* To begin with, if just a value, return the value. */
    if (token.type == ZFT_LITERAL) {

        zf_unlex(lexer, &token);
        if (zfp_parse_value(expr, lexer)) {
            code = ZFPI_SUB;
            goto error_out;
        }

        goto cont_check;

    }

    /* If just an identifier, return the identifier. */
    if (token.type == ZFT_IDENT) {

        zf_unlex(lexer, &token);
        if (zfp_parse_ident(expr, lexer)) {
            code = ZFPI_SUB;
            goto error_out;
        }

        goto cont_check;

    }

    /* If an expression in parentheses, swallow up the "(", parse an expression,
     * and swallow up the ")". If an operator follows, then turn the expr we
     * just parsed into the left-hand side of this operator.
     */

    /* Also, an aside - we do not need to worry about the order of operations
     * just yet. I'll code that later. Probably.
     */

    if (token.type == ZFT_OPAREN) {

        /* No unlex */

        if (zfp_parse_expr(expr, lexer)) {
            code = ZFPI_SUB;
            goto error_out;
        }

        /* ")" check */
        zf_lex(lexer, &token);
        if (token.type != ZFT_CPAREN) {
            ZFP_TOKEN_ERROR(lexer, ")", token);
            goto error_out;
        }

        goto cont_check;
        
    }

cont_check:
    /* Check if expression continues */
    zf_lex(lexer, &token);
    if (token.type == ZFT_OPERATOR) {
        /* Turn expr into left-hand side of operator */
        node->as.expr.left = expr;
        /* Operator parsing - TODO fix */
        strcpy(node->as.expr.opbuf, token.data);
        if ( !(node->as.expr.right = malloc(sizeof *node)) ) {
            ZF_PRINT_ERROR("Failed to allocate expression node.");
            code = ZFPI_ALLOC;
            goto error_out;
        }
        zfp_parse_expr(node->as.expr.right, lexer);
        goto done;
    } else {
        /* Expression does not continue. Copy intermediate value to node
            * and free malloc'd memory. */
        memcpy(node, expr, sizeof *node);
        /* Also remember to unlex the token */
        zf_unlex(lexer, &token);
        free(expr);
        goto done;
    }

error_out:
    free(expr);
    return code;

done:
    return ZFPI_GOOD;

}

static enum zfp_code
zfp_parse_decl(struct zfa_node * node, struct zf_lexer * lexer) {
    /* TODO */
}

static enum zfp_code
zfp_parse_funccall(struct zfa_node * node, struct zf_lexer * lexer) {
    /* TODO */
}

static enum zfp_code
zfp_parse_blockstmt(struct zfa_node * node, struct zf_lexer * lexer) {
    /* TODO */
}

static enum zfp_code
zfp_parse_function(struct zfa_node * node, struct zf_lexer * lexer) {
    /* TODO */
}
