/**
 * Lexer interface to receive abstract tokens from files.
 */

#pragma once

#include <stdio.h> /* FILE */

/* I could see really, really long quotes being about this long. */
#define ZF_TOKEN_LEX_MAX 512

/* The number of tokens in the put-back buffer. */
#define ZF_TOK_BUFSIZ    16

enum zf_token_type {

    ZFT_EOF,
    ZFT_IDENT,
    ZFT_LITERAL,
    ZFT_OPAREN,
    ZFT_CPAREN,
    ZFT_COMMA,
    ZFT_SEMICOLON,
    ZFT_COLON,
    ZFT_DOT,
    ZFT_OPERATOR,
    ZFT_OBRACKET,
    ZFT_CBRACKET,
    ZFT_OBRACE,
    ZFT_CBRACE,
    ZFT_MAX,

};

struct zf_token {

    /* The string content of the token. */
    char                      data[ZF_TOKEN_LEX_MAX];

    /* How long the token is. */
    unsigned                  len;

    /* What line number the token was on (for printing diagnostics and such) */
    unsigned                  lineno;

    /* What possibly-empty string the token was on */
    unsigned                  linepos;

    enum zf_token_type        type;

};

struct zf_lexer {

    /* The file we're lexing. */
    FILE                    * fp;

    /* Current line number and position. */
    unsigned                  lineno
                            , linepos;

    /**
     * If we unget a newline character, the line position will be the length of
     * the previous line, which is saved nowhere.
     */
    unsigned                  last_linepos;

    struct zf_token           tokbuf[ZF_TOK_BUFSIZ];
    int                       unlexed_count;

};

/**
 * Initialize a lexer from a file name. This prints and a diagnostic AND returns 
 * a nonzero number if lexer creation fails.
 */
unsigned zf_lexer_init(struct zf_lexer *, const char * filename);

/**
 * Fill a token with the next token from the file. Return 0 is successful, or 1
 * if EOF. (A better solution with token types will be here soon.) Return 2 if a
 * chaaracter is not recognized, or 3 if the token is too long.
 */
unsigned zf_lex(struct zf_lexer *, struct zf_token *);

/**
 * Place a token back into the input stream. Return 1 if error, of 0 if none.
 */
unsigned zf_unlex(struct zf_lexer *, struct zf_token *);

/**
 * Destroy a lexer.
 */
void zf_lexer_destroy(struct zf_lexer *);

/**
 * Get a string representation of a token type.s
 */
const char * zf_token_type_name(enum zf_token_type);
