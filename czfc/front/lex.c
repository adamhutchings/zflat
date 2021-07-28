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
        lexer->linepos = 0;
    } else {
        ++(lexer->linepos);
    }
    return c;
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
 * A function that tests if a character matches some criterion.
 */
typedef int                 (*zf_lex_test)(int c);

/**
 * Get the function for a given criterion. This only gets ONE criterion at a 
 * time - do not pass ZFL_WHITESPACE | ZFL_ALPHA.
 */
static zf_lex_test zf_lex_get_test(int flag) {

    switch (flag) {
        case ZFL_WHITESPACE:
            return isspace;
        case ZFL_ALPHA:
            return isalpha;
        case ZFL_DIGIT:
            return isdigit;
        case ZFL_CSYMBOL:
        /* This may nnot be 100% accurate - (see backslashes). We'll be able to
         * catch the error (hey! \ isn't an operator!) but a more thorough
         * solution may be in order later. */
            return ispunct;
        default:
            return NULL;
    }

}
