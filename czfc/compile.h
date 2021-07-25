/**
 * Interface for core compiler functions.
 */

#pragma once

#include <args.h>

/**
 * Run the actual compiler script using the given settings. This assumes that
 * zf_output_peripherals has already been run.
 */
void zf_execute_args(struct zf_args *);
