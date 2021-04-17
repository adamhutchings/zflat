# ZFC - internal workings

## Overview
This file aims to describe *how* the compiler works, because documentation up
until this point has been sparse.

## Entry point
The main function has some argument parsing, but then enters one of two
functions - `compile_one` or `compile_default` . These, in turn, both call the
same function slightly differently, the core `compile` function.

## AST structure
The structure of syntax trees in the compiler is simple in practice - one type
called `ASTNode` is the superclass of all syntax tree nodes, which contains a
method to construct itself from an input stream, and a method to write itself in
C form into an output stream. In addition, many of the AST node types have
pointers, or vectors of pointers, to child nodes (like a block statement to its
component statements). The `apply` method takes in an input function and
applies it to all children, as well as itself - this makes reordering expression
trees simple, as the reorder function is simply passed in as an argument.

## Compilation
The `compile` function constructs a syntax tree by reading one node of type
`ProgramNode` from a input file, reordering its component nodes, and writing it
to an output file. The output file may either be in C, or the compiler will
create a temporary C source file, write the code into it, compile the C code to
an object file, and then delete the temporary. Temporary names are made with
20 random digits to minimize the likelihood of a name collision.

## Operators, symbols, and control flow
Before, these were represented as plain strings in the AST nodes, but now there
are symbol, operator, and control flow classes / enums to make life easier.
The symbol files contain methods to resolve function / variable references by
name and argument types. Operator and control flow files contain simply methods
to convert to and from their respective enums to strings. Function names are
written to files like this:
`scope-name$$$function-name$arg-type-1$arg-type-2$$return-type`
Symbols are in symbol tables, which are a vector of symbols. These symbol tables
themselves are stored in a stack, where entering a block scope in the source
code adds a new scope to the stack. The bottom-most stack is import-level
definitions, the second is global definitions, and all other scopes are
temporary. Symbol resolution and other semantic checking is done during the
parsing stage of the compilation process, and not in its own step.
