/**
 * Lexer interface to receive abstract tokens from files.
 */

#pragma once

#include <stdio.h> /* FILE */

/* I could see really, really long quotes being about this long. */
#define ZF_TOKEN_LEX_MAX 512

struct zf_token {

    /* The string content of the token. */
    char                      data[ZF_TOKEN_LEX_MAX];

    /* What line number the token was on (for printing diagnostics and such) */
    unsigned                  lineno;

    /* What possibly-empty string the token was on */
    unsigned                  linepos;

    /* TODO - token types */

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

};

/**
 * Initialize a lexer from a file name. This prints and a diagnostic AND returns 
 * a nonzero number if lexer creation fails.
 */
unsigned zf_lexer_init(struct zf_lexer *, const char * filename);

/**
 * Fill a token with the next token from the file. Return 0 is successful, or 1
 * if EOF. (A better solution with token types will be here soon.)
 */
unsigned zf_lex(struct zf_lexer *, struct zf_token *);

/**
 * Destroy a lexer.
 */
void zf_lexer_destroy(struct zf_lexer *);
