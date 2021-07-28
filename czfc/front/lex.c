#include "lex.h"

#include <ctype.h>  /* isspace, etc */
#include <string.h> /* memset */

#include <output.h> /* error macros */

/**
 * Internal - get the next character.
 */
static int zf_lex_getc(struct zf_lexer * lexer) {
    int c = getc(lexer->fp);
    if (c == '\n') {
        ++(lexer->lineno);
        lexer->last_linepos = lexer->linepos;
        lexer->linepos = 0;
    } else {
        ++(lexer->linepos);
    }
    return c;
}

/**
 * Internal - unget character.
 */
static int zf_lex_ungetc(struct zf_lexer * lexer, int c) {
    if (c == '\n') {
        --(lexer->lineno);
        lexer->linepos = lexer->last_linepos;
        /* Hopefully we never have to unget across more than one line */
        lexer->last_linepos = 0;
    } else {
        --(lexer->linepos);
    }
}

unsigned zf_lexer_init(struct zf_lexer * lexer, const char * filename) {

    memset(lexer, 0, sizeof *lexer);

    /* Linepos start at 1, column start at 0 */
    lexer->linepos = 1;

    lexer->fp = fopen(filename, "r");
    if (lexer->fp == NULL) {
        ZF_PRINT_ERROR("Cannot open file \"%s\"", filename);
        return 1;
    }

    return 0;

}

void zf_lexer_destroy(struct zf_lexer * lexer) {
    fclose(lexer->fp);
}

/**
 * Internal use flags - these represent different types of characters.
 */
static enum {
    ZFL_WHITESPACE          = 0x01,
    ZFL_ALPHA               = 0x02,
    ZFL_DIGIT               = 0x04,
    ZFL_CSYMBOL             = 0x08,
};

/**
 * All character types which can begin a symbol.
 */
static int                    zf_token_begin_flags =
    ZFL_ALPHA | ZFL_DIGIT | ZFL_CSYMBOL;

/**
 * Character types which ALWAYS begin a symbol.
 */
static int                    zf_token_always_begin_flags =
    ZFL_CSYMBOL;

/**
 * See if a character matches ANY flags.
 */
static int zf_lex_test_any(int c, int flags) {

    if (flags & ZFL_WHITESPACE) {
        if (isspace(c)) return 1;
    }

    if (flags & ZFL_ALPHA) {
        if (isalpha(c)) return 1;
    }

    if (flags & ZFL_DIGIT) {
        if (isnumber(c)) return 1;
    }

    if (flags & ZFL_CSYMBOL) {
        if (ispunct(c)) return 1;
    }

    return 0;

}

/**
 * Skip whitespace. Return whether EOF was hit.
 */
static int zf_lex_skip_whitespace(struct zf_lexer * lexer) {

    int c;

    while (1) {
        c = zf_lex_getc(lexer);
        if (c == EOF) return 1;
        if (zf_lex_test_any(c, zf_token_begin_flags)) {
            /* We've read one character too far, one which begins a token. */
            zf_lex_ungetc(lexer, c);
            return 0;
        }
    }

}
