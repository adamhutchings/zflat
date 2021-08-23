#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <args.h>
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
 * Macro - set node to a newly alloc'd node - if allocation fails, print out an
 * error message using name. THIS FUNCTION WILL goto alloc_error; JUST BE AWARE.
 */
#define P_ALLOCNODE(node, name) do { \
    node = malloc(sizeof *node); /* Pool allocator in the future? */ \
    if (node == NULL) { \
        ZF_PRINT_ERROR("Failed to allocate " name " node."); \
        goto alloc_error; \
    } \
} while (0)

/**
 * Initialize lexer and after doing validation routines, pass control to the
 * actual core parsing code.
 */
struct zfa_node * zfp_parse(const char * filename, unsigned flags) {

    struct zf_lexer           lexer;
    struct zfa_node         * parse_tree;

    parse_tree = NULL;

    if (zf_lexer_init(&lexer, filename)) {
        ZF_PRINT_ERROR("Failed to create lexer.");
        return NULL;
    }

    if (flags & ZF_TOKEN_STREAM_FLAG) {

        struct zf_token       token;
        for (;;) {
            if (zf_lex(&lexer, &token) > 1) {
                ZF_PRINT_ERROR("Failed to get next token.");
                goto out;
            }
            if (token.type == ZFT_EOF) {
                goto out;
            }
            printf(
                "Content: %20s, type: %5d, lineno: %5d, linepos: %5d\n",
                token.data, token.type, token.lineno, token.linepos
            );
        }

    }

    P_ALLOCNODE(parse_tree, "program tree");
    
    /* Core parse */
    if (zfp_iparse(parse_tree, &lexer)) {
        ZF_PRINT_ERROR("parse failed");
    }
    goto out;

alloc_error:
    free(parse_tree);
    parse_tree = NULL;

out:
    return parse_tree;

}

/* Forward declarations for parse routines */
static enum zfp_code
zfp_parse_program      (struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_ident        (struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_value        (struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_expr         (struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_decl         (struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_blockstmt    (struct zfa_node * node, struct zf_lexer * lexer),
zfp_parse_function     (struct zfa_node * node, struct zf_lexer * lexer);

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
        P_ALLOCNODE(decl, "top-level declaration");

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

alloc_error:
    return ZFPI_ALLOC;

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

/**
 * Internal - parse a single value - this means a literal, a variable, or an
 * entire expression in parentheses.
 */
static enum zfp_code
zfp_parse_atomic_expr(struct zfa_node * node, struct zf_lexer * lexer) {

    struct zf_token           token;
    enum zfp_code             code;

    memset(node, 0, sizeof *node);

    zf_lex(lexer, &token);
    switch (token.type) {
    case ZFT_LITERAL:
        /* Parse a single literal. */
        zf_unlex(lexer, &token);
        if (zfp_parse_value(node, lexer)) {
            return ZFPI_SUB;
        }
        goto out;
    case ZFT_IDENT:
        /* Parse a single identifier. */
        zf_unlex(lexer, &token);
        if (zfp_parse_ident(node, lexer)) {
            return ZFPI_SUB;
        }
        goto out;
    case ZFT_OPAREN:
        /* Swallow the paren and parse an expr */
        if (zfp_parse_expr(node, lexer)) {
            return ZFPI_SUB;
        }
        /* Expect a closing paren. */
        zf_lex(lexer, &token);
        if (token.type != ZFT_CPAREN) {
            ZFP_TOKEN_ERROR(lexer, ")", token);
            return ZFPI_TOK;
        }
        goto out;
    default:
        ZFP_TOKEN_ERROR(lexer, "literal, identifier, or (", token);
        return ZFPI_TOK;
    }

out:
    code = ZFPI_GOOD;
    return code;

}

static enum zfp_code
zfp_parse_expr(struct zfa_node * node, struct zf_lexer * lexer) {

    /* TODO - operator precedence. Oh boy. */

    struct zf_token           token;
    
    /* expr stores the expression we're currently building, and expr2 stores the
     * current expression "in the pipeline."
     */
    struct zfa_node         * expr, * expr2;

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_EXPR;

    expr = NULL;

    for (;;) {

        /* Keep parsing atomic expressions, building a very one-sided tree. */

        /* If expr is not NULL, we've already seen an expression, so we expect
         * an operator. */
        if (expr) {
            zf_lex(lexer, &token);
            if (token.type != ZFT_OPERATOR) {
                zf_unlex(lexer, &token);
                goto out; /* We're done */
            }
        }

        /* Parse the next atomic expression. */
        P_ALLOCNODE(expr2, "expression");
        if (zfp_parse_atomic_expr(expr2, lexer)) {   
            return ZFPI_SUB;
        }

        /* if expr is NULL, no tree-formation is needed. */
        if (!expr) {
            expr = expr2;
        } else {

            struct zfa_node * expr3;

            /* Save value of expr. */
            expr3 = expr;

            P_ALLOCNODE(expr, "expression");

            expr->as.expr.left = expr3;

            if (token.len >= OPBUF_SIZE) {
                ZF_PRINT_ERROR("operator too long");
                return ZFPI_BUF;
            }
            memcpy(expr->as.expr.opbuf, token.data, token.len);

            expr->as.expr.right = expr2;

        }

    }

alloc_error:
    return ZFPI_ALLOC;

out:
    node->as.expr.left = expr->as.expr.left;
    node->as.expr.right = expr->as.expr.right;
    memcpy(node->as.expr.opbuf, expr->as.expr.opbuf, OPBUF_SIZE);

    return ZFPI_GOOD;

}

static enum zfp_code
zfp_parse_decl(struct zfa_node * node, struct zf_lexer * lexer) {

    struct zfa_node          * type, * expr;
    struct zf_token            token, ident;

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_DECL;

    zf_lex(lexer, &ident);
    if (ident.type != ZFT_IDENT) {
        ZFP_TOKEN_ERROR(lexer, "identifier", ident);
        return ZFPI_TOK;
    }
    if (strlen(token.data) >= ZF_IDENT_MAXLEN) {
        ZF_PRINT_ERROR("line %d: buffer overflow in token name", lexer->lineno);
        return ZFPI_BUF;
    }
    strcpy(node->as.decl.namebuf, token.data);
    node->as.decl.namebuf_len = token.len;

    /* Check for colon */
    zf_lex(lexer, &token);
    if (token.type != ZFT_COLON) {
        ZFP_TOKEN_ERROR(lexer, ":", token);
        return ZFPI_TOK;
    }

    /* Type name */
    P_ALLOCNODE(type, "type");
    if (zfp_parse_ident(type, lexer)) {
        return ZFPI_SUB;
    }
    
    if (type->as.ident.namebuf_len > TYPE_MAX_LEN) {
        ZF_PRINT_ERROR("line %d: type too long", lexer->lineno);
        return ZFPI_BUF;
    }
    strcpy(node->as.decl.typebuf, type->as.ident.namebuf);
    node->as.decl.typebuf_len = type->as.ident.namebuf_len;

    /* See whether there's an expr following or not. */
    zf_lex(lexer, &token);
    if (token.type == ZFT_OPERATOR) {

        /* TODO - better check for equals sign */
        if (strcmp(token.data, "=")) {
            ZFP_TOKEN_ERROR(lexer, "=", token);
            return ZFPI_TOK;
        }

        /* Parse expression */
        P_ALLOCNODE(expr, "expression");
        if (zfp_parse_expr(expr, lexer)) {
            return ZFPI_SUB;
        }

        node->as.decl.expr = expr;

    } else {
        zf_unlex(lexer, &token);
    }

    /**
     * Expect a semicolon at the end. This is necessary because these can be
     * top-level decls.
     */
    zf_lex(lexer, &token);
    if (token.type != ZFT_SEMICOLON) {
        ZFP_TOKEN_ERROR(lexer, ";", token);
        return ZFPI_TOK;
    }

    return ZFPI_GOOD;

alloc_error:
    return ZFPI_ALLOC;

}

static enum zfp_code
zfp_parse_blockstmt(struct zfa_node * node, struct zf_lexer * lexer) {
    
    struct zfa_node          * stmt;
    struct zf_token            token, token2;

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_BLOCK_STMT;

    zf_lex(lexer, &token);
    if (token.type != ZFT_OBRACE) {
        ZFP_TOKEN_ERROR(lexer, "{", token);
        return ZFPI_TOK;
    }

    zfll_init(&node->as.blockstmt.stmts);

    for (;;) {

        zf_lex(lexer, &token);
        if (token.type == ZFT_CBRACE) {
            break;  
        } else {
            zf_unlex(lexer, &token);
        }

        P_ALLOCNODE(stmt, "statement");

        /* Parse an expression */

        /* The "expression" is a declaration, IF token 2 is a ":" . */
        zf_lex(lexer, &token);
        zf_lex(lexer, &token2);
        zf_unlex(lexer, &token2);
        zf_unlex(lexer, &token);
        if (token2.type == ZFT_COLON) {
            if (zfp_parse_decl(stmt, lexer)) {
                return ZFPI_SUB;
            }
        } else {

            /* No colon. This is an expression. */

            /* OH WAIT! Maybe it's another blockstmt. */
            if (token.type == ZFT_OBRACE) {
                if (zfp_parse_blockstmt(stmt, lexer)) {
                    return ZFPI_SUB;
                }
            }

            if (zfp_parse_expr(stmt, lexer)) {
                return ZFPI_SUB;
            }

            zfll_add(&node->as.blockstmt.stmts, stmt);

            /* Expect a semicolon after the expression. */
            zf_lex(lexer, &token);
            if (token.type != ZFT_SEMICOLON
            &&  stmt->type != ZFA_NODE_DECL
            &&  stmt->type != ZFA_NODE_BLOCK_STMT) {
                ZFP_TOKEN_ERROR(lexer, ";", token);
                return ZFPI_TOK;
            }

        }

    }

    return ZFPI_GOOD;

alloc_error:
    return ZFPI_ALLOC;

}

static enum zfp_code
zfp_parse_function(struct zfa_node * node, struct zf_lexer * lexer) {
    
    struct zf_token            token;
    int                        params;
    struct zfa_node          * param;

    memset(node, 0, sizeof *node);
    node->type = ZFA_NODE_FUNCTION;

    zf_lex(lexer, &token);
    if (token.type != ZFT_IDENT) {
        ZFP_TOKEN_ERROR(lexer, "identifier", token);
        return ZFPI_TOK;
    }
    if (strlen(token.data) >= ZF_IDENT_MAXLEN) {
        ZF_PRINT_ERROR("line %d: buffer overflow in token name", lexer->lineno);
        return ZFPI_BUF;
    }
    strcpy(node->as.function.decl->as.decl.namebuf, token.data);
    node->as.function.decl->as.decl.namebuf_len = token.len;

    zf_lex(lexer, &token);
    if (token.type != ZFT_OPAREN) {
        ZFP_TOKEN_ERROR(lexer, "(", token);
        return ZFPI_TOK;
    }

    zfll_init(&node->as.function.params);

    for (params = 0; ; ++params) {

        /* end of paramslist check */
        zf_lex(lexer, &token);
        if (token.type == ZFT_CPAREN) {
            break;
        }
        zf_unlex(lexer, &token);

        /* Comma if we're not at the beginning of the list. */
        if (params > 0) {
            zf_lex(lexer, &token);
            if (token.type != ZFT_COMMA) {
                ZFP_TOKEN_ERROR(lexer, ",", token);
                return ZFPI_TOK;
            }
        }

        /* Parse a param */
        P_ALLOCNODE(param, "function parameter");

        if (zfp_parse_decl(param, lexer)) {
            return ZFPI_SUB;
        }
        zfll_add(&node->as.function.params, param);

    }

    /* Get return type */
    zf_lex(lexer, &token);
    if (token.type != ZFT_COLON) {
        ZFP_TOKEN_ERROR(lexer, ":", token);
        return ZFPI_TOK;
    }
    /* TODO - accept functions without return types */
    zf_lex(lexer, &token);
    if (token.type != ZFT_IDENT) {
        ZFP_TOKEN_ERROR(lexer, "type name", token);
        return ZFPI_TOK;
    }

    if (strlen(token.data) >= ZF_IDENT_MAXLEN) {
        ZF_PRINT_ERROR("line %d: buffer overflow in ret type", lexer->lineno);
        return ZFPI_BUF;
    }
    strcpy(node->as.function.decl->as.decl.typebuf, token.data);
    node->as.function.decl->as.decl.typebuf_len = token.len;

    P_ALLOCNODE(node->as.function.body, "function body");
    if (zfp_parse_blockstmt(node->as.function.body, lexer)) {
        return ZFPI_SUB;
    }

    return ZFPI_GOOD;

alloc_error:
    return ZFPI_ALLOC;

}
