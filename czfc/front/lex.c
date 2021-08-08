#include "lex.h"

#include <ctype.h>  /* isspace, etc */
#include <string.h> /* memset */

#include <output.h> /* error macros */

/**
 * Internal - determine the type of a character buffer.
 */
static enum zf_token_type zf_lex_type(const char * buf) {

    char ini; /* The start char */

    /* TODO - improve this whole process */

    ini = *buf;

    if (ini == '\0') {
        /* Shouldn't need to use this */
        return ZFT_EOF;
    }

    if (isalpha(ini)) {
        return ZFT_IDENT;
    }

    if (isdigit(ini)) {
        /* Later, check for quotes, etc. */
        return ZFT_LITERAL;
    }

    if (ini == '(')
        return ZFT_OPAREN;

    if (ini == ')')
        return ZFT_CPAREN;

    if (ini == ',')
        return ZFT_COMMA;

    if (ini == ';')
        return ZFT_SEMICOLON;

    if (ini == ':')
        return ZFT_COLON;

    if (ini == '.')
        return ZFT_DOT;

    if (ini == '[')
        return ZFT_OBRACKET;

    if (ini == ']')
        return ZFT_CBRACKET;

    if (ini == '{')
        return ZFT_OBRACE;

    if (ini == '}')
        return ZFT_CBRACE;

    if (ispunct(ini))
        return ZFT_OPERATOR;

    return ZFT_IDENT; /* As in zfc. when in doubt ... */

}

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
static void zf_lex_ungetc(struct zf_lexer * lexer, int c) {
    if (c == '\n') {
        --(lexer->lineno);
        lexer->linepos = lexer->last_linepos;
        /* Hopefully we never have to unget across more than one line */
        lexer->last_linepos = 0;
    } else {
        --(lexer->linepos);
    }
    ungetc(c, lexer->fp);
}

unsigned zf_lexer_init(struct zf_lexer * lexer, const char * filename) {

    memset(lexer, 0, sizeof *lexer);

    /* Linepos start at 1, column start at 0 */
    lexer->linepos = lexer->lineno = 1;

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
enum {
    ZFL_WHITESPACE          = 0x01,
    ZFL_ALPHA               = 0x02,
    ZFL_DIGIT               = 0x04,
    ZFL_CSYMBOL             = 0x08,
    ZFL_EOF                 = 0x10,
};

/**
 * All character types which can begin a symbol.
 */
static int                    zf_token_begin_flags =
    ZFL_ALPHA | ZFL_DIGIT | ZFL_CSYMBOL;

/**
 * Character types which ALWAYS begin a symbol. Unused for now.
 */
/* static int                    zf_token_always_begin_flags =
    ZFL_CSYMBOL; */

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

    if (flags & ZFL_EOF) {
        if (c == EOF) return 1;
    }

    return 0;

}

/**
 * Skip whitespace. Return whether any whitespace was skipped.
 */
static int zf_lex_skip_whitespace(struct zf_lexer * lexer) {

    int c;
    int whitespace_count;

    whitespace_count = 0;

    while (1) {
        c = zf_lex_getc(lexer);
        if (c == EOF) goto out;
        if (zf_lex_test_any(c, zf_token_begin_flags)) {
            /* We've read one character too far, one which begins a token. */
            zf_lex_ungetc(lexer, c);
            goto out;
        }
        ++whitespace_count;
    }

out:
    return whitespace_count;

}

/**
 * Skip comment, if any. Return whether any comment was skipped.
 */
static int zf_lex_skip_comment(struct zf_lexer * lexer) {

    int c;

    if ( (c = zf_lex_getc(lexer)) != '~') {
        zf_lex_ungetc(lexer, c);
        return 0;
    }

    while (zf_lex_getc(lexer) != '~');
    return 1;

}

/**
 * Skip all whitespace and comments.
 */
static void zf_lex_skip_whitespace_and_comments(struct zf_lexer * lexer) {

    int any_skipped = 0;

    while (1) {
        any_skipped |= zf_lex_skip_whitespace(lexer);
        any_skipped |= zf_lex_skip_comment(lexer);
        if (!any_skipped) {
            return;
        } else {
            any_skipped = 0;
        }
    }

}

/**
 * Which characters are past the END of a token?
 */
static int zf_lex_end(int flag) {

    switch (flag) {
        case ZFL_ALPHA:
            return ZFL_WHITESPACE | ZFL_CSYMBOL | ZFL_EOF;
        case ZFL_DIGIT:
            return ZFL_WHITESPACE | ZFL_CSYMBOL| ZFL_EOF;
        case ZFL_CSYMBOL:
            /* Anything */
            return ZFL_WHITESPACE | ZFL_ALPHA | ZFL_DIGIT
            | ZFL_CSYMBOL | ZFL_EOF;
    }

    return 0;

}

/**
 * Finally, main lexer routine. This actually reads a token and returns it, as
 * opposed to the main lexer routine which checks the token buffer.
 */
static unsigned zf_ilex(struct zf_lexer * lexer, struct zf_token * tok) {

    /* What type is the beginning of the token? */
    int begin_token_type;

    /* Beginning character */
    int begin_token_char;

    /* Which flags can end a token */
    int end_flags;

    /* Skip whitespace BEFORE, not after, because a file can begin with
     * whitespace. */
    zf_lex_skip_whitespace_and_comments(lexer);

    memset(tok, 0, sizeof *tok);

    begin_token_char = zf_lex_getc(lexer);
    /* No longer a redundant check. */
    if (begin_token_char == EOF) {
        tok->type = ZFT_EOF;
        strcpy(tok->data, "[ eof ]");
        return 1;
    }
    /* The function-to-type correspondences here are the same as above -
     * fix later? */
    if (isspace(begin_token_char))       begin_token_type = ZFL_WHITESPACE;
    else if (isalpha(begin_token_char))  begin_token_type = ZFL_ALPHA;
    else if (isnumber(begin_token_char)) begin_token_type = ZFL_DIGIT;
    else if (ispunct(begin_token_char))  begin_token_type = ZFL_CSYMBOL;
    else
        return 2; /* Ivalid character encountered. */

    end_flags = zf_lex_end(begin_token_type);

    /* Fill the token's linepos, etc here, at the beginning. */
    tok->linepos = lexer->linepos;
    tok->lineno = lexer->lineno;

    /* Reuse the begin_token_char variable to store the current character. */
    do {

        /* Make sure we won't overflow the token buffer. */
        if (tok->len >= ZF_TOKEN_LEX_MAX - 1) {
            /* -1 to leave room for the null terminator. */
            return 3;
        }

        tok->data[tok->len++] = begin_token_char;

        begin_token_char = zf_lex_getc(lexer);
        if (zf_lex_test_any(begin_token_char, end_flags)) {
            /* End of token passed! */
            zf_lex_ungetc(lexer, begin_token_char);
            /* Before we return, cap off data with nul. */
            tok->data[tok->len] = '\0';
            tok->type = zf_lex_type(tok->data);
            return 0;
        }

    } while (1);

}

unsigned zf_lex(struct zf_lexer * lexer, struct zf_token * token) {

    if (lexer->unlexed_count) {
        memcpy(token, lexer->tokbuf + lexer->unlexed_count - 1, sizeof *token);
        --(lexer->unlexed_count);
        return 0;
    }

    return zf_ilex(lexer, token);

}

unsigned zf_unlex(struct zf_lexer * lexer, struct zf_token * token) {

    if (lexer->unlexed_count >= ZF_TOKEN_LEX_MAX) {
        return 1;
    }

    memcpy(lexer->tokbuf + lexer->unlexed_count, token, sizeof *token);
    ++(lexer->unlexed_count);
    return 0;

}
