/**
 * Command-line arguments to the application. This file describes the struct
 * used to parse them, as well as the methods to parse them from a list of
 * arguments.
 */

#pragma once

/**
 * The maximum number of files that can be passed as arguments to compile.
 */
#define MAX_FILES_TO_COMPILE  256

/**
 * The maximum number of errors that can be stored.
 */
#define MAX_PARSE_ERRORS      8

#define OUTPUT_FILE_LEN_MAX   256

/**
 * All types of errors that can occur.
 */
enum zf_args_error {

    ZF_TOO_MANY_ERRORS,
    ZF_TOO_MANY_FILES,
    
    ZF_ARGS_ERROR_MAX,

};

/**
 * All miscellaneous flags.
 */
#define ZF_HELP_FLAG          0x01
#define ZF_VERSION_FLAG       0x02

/* More will come later, no doubt. */


struct zf_args {
    
    /* The files to compile. Each entry here has the input and output
     * file paths.
     */
    struct                    zf_core_compiler_inputs {
        char                * input_file;
        char                  output_file [ OUTPUT_FILE_LEN_MAX ];
    }                         files_to_compile [MAX_FILES_TO_COMPILE];
    int                       nr_files_to_compile;

    struct zf_parse_error_diagnostic {
        /* Which type of error it is. */
        enum zf_args_error    error;
        /* Which argument the error occurred on. */
        int                   position;
    }                         errors [ MAX_PARSE_ERRORS ];
    int                       nr_errors;

    unsigned                  flags;

};

/**
 * Parse command-line arguments. The argv pointer passed in must NOT include
 * the program name.
 */
void zf_args_parse (struct zf_args * args, int argc, char ** argv);

/**
 * Stuck here - this is the program name so that any error macro can use it.
 */
extern char                 * zf_program_name;

/**
 * Initialize the program name. Pass in a pointer to the program name and arg
 * count, so they can be updated accordingly.
 */
void zf_args_init_program_name (char ** * program_namep, int * argcp);

/**
 * Output all non-compiler options (errors, version number, etc.) to stderr.
 * Pass in the command-line arguments so the error-causing options can be
 * displayed.
 */
void zf_output_peripherals(struct zf_args * args, char ** argv);
