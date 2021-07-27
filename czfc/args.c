#include "args.h"

#include <stdio.h>
#include <string.h>

#include <output.h>

/**
 * External reference from header.
 */
char                        * zf_program_name;

void zf_args_init_program_name (char ** * program_namep, int * argcp) {

    zf_program_name = (*program_namep)[0];

    /* Update argc and argv */
    ++*program_namep, --*argcp;

}

/**
 * Internal - add an error message to the error list. Return 1 if the list has
 * been capped with the max error value. position is the position of the bad
 * arggument in argv.
 */
int zf_args_add_error (struct zf_args * args, enum zf_args_error error, int position) {

    int ret;

    ret = 0;

    if (args->nr_errors >= MAX_PARSE_ERRORS) {
        goto error;
    }

    if (args->nr_errors == MAX_PARSE_ERRORS - 1) {
        /* Cap the error list */
        args->errors[args->nr_errors] =
            (struct zf_parse_error_diagnostic) { ZF_TOO_MANY_ERRORS, position };
        goto error;
    }

    args->errors[args->nr_errors] = (struct zf_parse_error_diagnostic) { error, position };
    
error:
    ret = 1;
out:
    ++args->nr_errors;
    return ret;

}

void zf_args_parse (struct zf_args * args, int argc, char ** argv) {

    int i; /* Argument index */

    memset( args, 0, sizeof( struct zf_args ) );

    for (i = 0; i < argc; ++i) {

        /* For now no flags exist. */
        if (args->nr_files_to_compile >= MAX_FILES_TO_COMPILE) {

            zf_args_add_error(args, ZF_TOO_MANY_FILES, i);

        } else {

            /* Add file to list of files to compile. */
            args->files_to_compile[args->nr_files_to_compile++].input_file
                = argv[i];
            /* The output file can be automatically determined by the driver. */
            args->files_to_compile[args->nr_files_to_compile-1].output_file[0] = '\0';

        }

    }

}

/**
 * Get an error message from an error code.
 */
static const char * get_error_message(enum zf_args_error code);

void zf_output_peripherals(struct zf_args * args, char ** argv) {

    int i;

    for (i = 0; i < args->nr_errors; ++i) {
        ZF_PRINT_WITH_COLOR(ZF_CODE_BLUE,
        "czfc: %s (detected at argument %s)\n",
            get_error_message(args->errors[i].error),
            argv[i]
        );
    }

}

static const char * get_error_message(enum zf_args_error code) {

    /**
     * Message array
     */
    static const char * error_messages[ZF_ARGS_ERROR_MAX] = {
        [ZF_TOO_MANY_ERRORS] = "Too many errors detected, stopping count",
        [ZF_TOO_MANY_FILES]  = "Too many files passed in",
    };

    return error_messages[code];

}
