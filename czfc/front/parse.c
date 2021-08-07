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

    /* Default */
    code = ZFPI_GOOD;
    
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
    } else if (token.type == ZFT_OPAREN) {
        /* expr is a function call. */
        node->as.funccall.ident = expr;
        int paramct;
        zfll_init(&node->as.funccall.params);
        for (paramct = 0; ; ++paramct) {
            zf_lex(lexer, &token);
            if (token.type == ZFT_CPAREN) {
                break;
            } else {
                zf_unlex(lexer, &token);
            }
            if (paramct) {
                /* Expect a comma */
                zf_lex(lexer, &token);
                if (token.type != ZFT_COMMA) {
                    ZFP_TOKEN_ERROR(lexer, ",", token);
                    code = ZFPI_TOK;
                    goto error_out;
                }
            }
            expr = malloc(sizeof *expr);
            if (!expr) {
                ZF_PRINT_ERROR("Failed to allocate expression node.");
                code = ZFPI_ALLOC;
                goto error_out;
            }
            if (zfp_parse_expr(expr, lexer)) {
                code = ZFPI_SUB;
                goto error_out;
            }
            zfll_add(&node->as.funccall.params, expr);
        }
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
    /* Okay, this treats types like identifiers, but types themselves are stored
     * as a character buffer. We need to fix this.
     */
    type = malloc(sizeof *type);
    if (zfp_parse_ident(type, lexer)) {
        return ZFPI_SUB;
    }
    if (!type) {
        ZF_PRINT_ERROR("Failed to allocate type node.");
        return ZFPI_ALLOC;
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
        expr = malloc(sizeof *expr);
        if (!expr) {
            ZF_PRINT_ERROR("Failed to allocate expression node.");
            return ZFPI_ALLOC;
        }
        if (zfp_parse_expr(expr, lexer)) {
            return ZFPI_SUB;
        }

        node->as.decl.expr = expr;

    } else {
        zf_unlex(lexer, &token);
    }

    return ZFPI_GOOD;

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

        stmt = malloc(sizeof *stmt);
        if (!stmt) {
            ZF_PRINT_ERROR("Failed to allocate statement node.");
            return ZFPI_ALLOC;
        }

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
            if (token.type != ZFT_SEMICOLON) {
                ZFP_TOKEN_ERROR(lexer, ";", token);
                return ZFPI_TOK;
            }

        }

    }

    return ZFPI_GOOD;

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
    strcpy(node->as.function.decl.namebuf, token.data);
    node->as.function.decl.namebuf_len = token.len;

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
            zf_unlex(lexer, &token);
        }

        /* Parse a param */
        param = malloc(sizeof *param);
        if (!param) {
            ZF_PRINT_ERROR("Failed to allocate parameter node.");
            return ZFPI_ALLOC;
        }

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
    strcpy(node->as.function.decl.typebuf, token.data);
    node->as.function.decl.typebuf_len = token.len;

    zfp_parse_blockstmt(node->as.function.body, lexer);

    return ZFPI_GOOD;


}
