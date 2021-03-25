/**
 * Error - error routines.
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <util/fs.hpp>

#define ZF_ARGS(...) , ##__VA_ARGS__

#define ZF_ERROR(msg, ...) do { \
    fprintf(stderr, "zfc: "); \
    fprintf(stderr, msg "\n" ZF_ARGS(__VA_ARGS__)); \
    fs::ifile.close();                              \
    fs::ofile.close();                              \
    exit(-1); \
} while (0)
