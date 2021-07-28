#include <compile.h>

#include <stdio.h>  /* printf */
#include <string.h> /* strncpy */

#include <front/lex.h>
#include <output.h>

/**
 * Fill buf with the default output file name. buf is assumed to hold exactly
 * OUTPUT_FILE_LEN_MAX characters.
 */
void default_output_file_name(const char * inputfile, char * buf) {

    /* Whether the file name is too long to add an extension, possibly */
    int file_name_long;

    strncpy(buf, inputfile, OUTPUT_FILE_LEN_MAX);

    /* Replace .zf with .c */
    char * dot = strrchr(buf, '.');

    file_name_long = dot - buf >= OUTPUT_FILE_LEN_MAX - 2;

    if (file_name_long) {
        ZF_PRINT_WARNING("File name is too long to have a proper extension");
    }

    if (dot == NULL) {
        ZF_PRINT_WARNING("File name \"%s\" has no extension", buf);
    }

    if (dot[1] != 'z' || dot[2] != 'f') {
        ZF_PRINT_WARNING("File name does not have ending .zf,"
        "has \".%s\" instead", dot + 1);
    }

    if (!file_name_long) {
        dot[1] = 'c';
        dot[2] = '\0'; /* cap with .c */
    } else {
         /* cap with .c at the end of the buffer */
        buf[OUTPUT_FILE_LEN_MAX - 2] = 'c';
        buf[OUTPUT_FILE_LEN_MAX - 1] = '\0';
    }

}

/**
 * ACTUALLY call the compilation functions. Return 0 if succeeded, 1 otherwise.
 */
static int zf_compile(char * inputfile, char * outputfile) {

    struct zf_lexer           lexer;
    struct zf_token           token;
    int                       ret;

    ret = 0; /* Default */

    if (zf_lexer_init(&lexer, inputfile)) {
        ZF_PRINT_ERROR("Lexer initialization failed.");
        goto error;
    }

lex_loop:
    switch (zf_lex(&lexer, &token)) {
    case 0: /* Good */
        printf(
            "Token: %20s, line: %5d, linepos: %5d\n",
            token.data, token.lineno, token.linepos
        );
        goto lex_loop;
    case 1:
        printf("End-of-file detected.\n");
        break;
    case 2:
        printf("Unrecognized character.\n");
        break;
    case 3:
        printf("Buffer overflow.\n");
        break;
    }

error:
    ret = 1;

out:
    zf_lexer_destroy(&lexer);
    return ret;

}

void zf_execute_args(struct zf_args * args) {

    int i;

    for (i = 0; i < args->nr_files_to_compile; ++i) {
        /* Get default output name if none exists. */
        if (args->files_to_compile[i].output_file[0] == '\0') {
            default_output_file_name(
                args->files_to_compile[i].input_file,
                args->files_to_compile[i].output_file
            );
        }
    }

    /**
     * Compile all files.
     */
    for (i = 0; i < args->nr_files_to_compile; ++i) {
        if (zf_compile(
            args->files_to_compile[i].input_file,
            args->files_to_compile[i].output_file
        )) {
            ZF_PRINT_ERROR("Compilation failed.");
        }
    }

}
