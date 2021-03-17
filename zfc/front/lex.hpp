/**
 * lex - get tokens.
 */

#pragma once

#include <fstream>
#include <string>

enum TokenType {
    IDENTIFIER,         /* a variable or function name */
    TYPENAME,           /* int, or a user-defined name */
    OPERATOR,           /* any operator */
    COLON,              /* : */
    OPAREN,             /* ( */
    CPAREN,             /* ) */
    OBRACE,             /* { */
    CBRACE,             /* } */
    OBRACKET,           /* [ */
    CBRACKET,           /* ] */
    CFLOW,              /* return, break, continue */
    SCOLON,             /* ; */
    LITERAL,            /* "string", 'char', 1234, etc */
    TEOF
};

struct Token {
    TokenType type;
    std::string str;
};

/**
 * Stuff tok with the next token.
 */
Token& lex(std::ifstream& file, Token& tok);

extern int tokens;
