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

/**
 * All types of errors that can occur.
 */
enum zf_args_error {

};

struct zf_args {
    
    /* The files to compile. Each entry here has the input and output
     * file paths.
     */
    struct                    zf_core_compiler_inputs {
        char                * input_file;
        char                * output_file;
    }                         files_to_compile [MAX_FILES_TO_COMPILE];
    int                       nr_files_to_compile;

    struct {
        /* Which type of error it is. */
        enum zf_args_error    error;
        /* Which argument the error occurred on. */
        int                   position;
    }                         errors [ MAX_PARSE_ERRORS ];
    int                       nr_errors;

};

/**
 * Parse command-line arguments. The argv pointer passed in must NOT include
 * the program name.
 */
void zf_args_parse ( struct zf_args * args, int argc, char ** argv );

/**
 * Stuck here - this is the program name so that any error macro can use it.
 */
extern char                 * zf_program_name;

/**
 * Initialize the program name. Pass in a pointer to the program name and arg
 * count, so they can be updated accordingly.
 */
void zf_args_init_program_name ( char ** * program_namep, int * argcp );
