/**
 * Lexer interface to receive abstract tokens from files.
 */

#pragma once

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

};
