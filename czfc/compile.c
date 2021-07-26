#include <compile.h>

#include <string.h> /* strncpy */

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
        /* Warn about a file path being possibly too long */
    }

    if (dot == NULL) {
        /* Warn about no extension */
    }

    if (dot[1] != 'z' || dot[2] != 'f') {
        /* Warn about non-zf filename */
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

void zf_execute_args(struct zf_args * args) {



}
