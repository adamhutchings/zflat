#include "lex.h"

#include <string.h> /* memset */

#include <output.h> /* error macros */

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
