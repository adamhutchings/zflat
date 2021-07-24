#include "args.h"

#include <stdio.h>
#include <string.h>

/**
 * External reference from header.
 */
char                        * zf_program_name;

void zf_args_init_program_name (char ** * program_namep, int * argcp) {

    zf_program_name = (*program_namep)[0];

    /* Update argc and argv */
    ++*program_namep, --*argcp;

}

void zf_args_parse (struct zf_args * args, int argc, char ** argv) {

    int i; /* Argument index */

    memset( args, 0, sizeof( struct zf_args ) );

    for (i = 0; i < argc; ++i) {

        /* For now no flags exist. */
        if (args->nr_files_to_compile >= MAX_FILES_TO_COMPILE) {

            /* Try to add an error. */
            if (args->nr_errors >= MAX_PARSE_ERRORS - 1) {
                /* If there's only one error slot left, add "too many errors."
                as a flag. */
                args->errors[args->nr_errors++] =
                    (struct zf_parse_error_diagnostic) { ZF_TOO_MANY_ERRORS, i };
                return;
            } else {
                args->errors[args->nr_errors++] =
                    (struct zf_parse_error_diagnostic) { ZF_TOO_MANY_FILES, i };
            }

        } else {

            /* Add file to list of files to compile. 
            TODO - have an actual output file name. */
            args->files_to_compile[args->nr_files_to_compile++] = 
                (struct zf_core_compiler_inputs) { argv[i], "out.c" };

        }

    }

}

/**
 * Get an error message from an error code.
 */
static char * get_error_message(enum zf_args_error code);

void zf_output_peripherals(struct zf_args * args, char ** argv) {

    int i;

    for (i = 0; i < args->nr_errors; ++i) {
        fprintf(
            stderr, "czfc: %s (detected at argument %s)\n",
            get_error_message(args->errors[i].error),
            argv[i]
        );
    }

}

static char * get_error_message(enum zf_args_error code) {

    /**
     * Message array
     */
    static const char * error_messages[ZF_ARGS_ERROR_MAX] = {
        [ZF_TOO_MANY_ERRORS] = "Too many errors detected, stopping count",
        [ZF_TOO_MANY_FILES]  = "Too many files passed in",
    };

    return error_messages[code];

}
