# CONTRIBUTING TO Z-FLAT

## Overview
Perhaps you'd be interested in implementing a new feature, or in fixing either a
known bug or one you found yourself. Either way, you can create a pull request,
but if it is a new feature, make sure to ask Adam Hutchings.

## Code format
- Opening braces DO NOT go on their own line.
- The `*` or `&` is attached to the type name - `type* var`. The exception is if
you are creating multiple variables - `type *var1, *var2`.
- Add spaces around multiply-parenthesized expressions. Don't do `(((2+3)*(4-5))*7)`,
do `( ( ( 2+3 ) * ( 4-5 ) ) * 7 )` . If it's too complicated, consider breaking
it up on multiple lines.
- Use common sense. And the English language.

## PR Title
Use \[general area\]: \[overall message\], as in `lex: add 'foo' as token`.
