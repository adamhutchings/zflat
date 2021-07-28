/**
 * czfc main file.
 */

#include <args.h>
#include <compile.h>

int main(int argc, char ** argv) {

    struct zf_args args;

    zf_args_init_program_name(&argv, &argc);
    zf_args_parse(&args, argc, argv);
    zf_output_peripherals(&args, argv);
    zf_execute_args(&args);

}
