/**
 * Define outputting diagnostics to terminal.
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

#define ZF_CODE_RED 31
#define ZF_CODE_YELLOW 33
#define ZF_CODE_GREEN 32
#define ZF_CODE_BLUE 34
#define ZF_CODE_MAGENTA 35
#define ZF_CODE_CYAN 36
#define ZF_CODE_WHITE 37

/**
 * Output stream is stderr.
 */

#define ZF_OUTPUT_STREAM stderr

#define ZF_SET_COLOR(color) fprintf(ZF_OUTPUT_STREAM, "\033[%dm", color)

#define ZF_RESET_COLOR(color) fprintf(ZF_OUTPUT_STREAM, "\033[0m")

#define ZF_PRINT_WITH_COLOR(color, ...) do { \
    ZF_SET_COLOR(color); \
    fprintf(ZF_OUTPUT_STREAM, __VA_ARGS__); \
    ZF_RESET_COLOR(color); \
} while (0)

#define ZF_ERROR_OUT(...) do { \
    ZF_PRINT_WITH_COLOR(ZF_CODE_RED, __VA_ARGS__); \
    exit(-1); \
} while (0)
