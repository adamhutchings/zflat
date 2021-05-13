# Compiler

## About the compiler
The `zfc` compiler takes in Z-flat source code and outputs either C output code,
or object code (by compiling the C code). By default, the compiler searches the
current directory for an `src` directory, and tries to find all `*.zf` files
inside it - by default, it compiles each `src/*.zf` to `bin/*.o`.

## Compiler flags

`-I [file]` - compile just the single file `file`, by default to the same path
with `.o` instead of `.zf`.
`-c` - instead of generating a randomly-named C file and then compiling it to
object code, generate a file with the same name except for `.c` at the end
instead of `.zf`, and do not generate object code.
`--help` - generate a help message and exit.
`--debug=[num]` - set the debug level to `[num]` (must be between 0 and 4).
`--release` - set the debug value to 0 (is 1 by default)
