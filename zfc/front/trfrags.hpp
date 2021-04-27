/**
 * Tree fragments - types for tokens and tree fragments.
 */

#pragma once

enum TreeComp {

    COMMENT, // blank, discarded later

    // Errors
    EGENERALERROR,
    ETOOLARGE,
    EOPENQUOTE,
    EOPENCOMMENT,

    // Token types
    IDENTIFIER,
    TYPENAME,
    LITERAL,
    OPERATOR,
    SEMICOLON,
    OBRACE,
    CBRACE,
    OPAREN,
    CPAREN,
    OBRACKET,
    CBRACKET,
    COLON,
    COMMA,
    DOT,
    FLOW, // break, continue return
    LOOP, // loop
    IF,
    ELSE,
    SWITCH,
    FSWITCH,
    CASE,
    EXTC,
    VA_ARGS,
    USE,
    REF,

    // Tree fragments
    EXPRESSION,
    VAR_DECL,
    FLOW_STATEMENT, // break, return, continue
    STATEMENT,
    BLOCK_STATEMENT,
    FUNCTION_DECL,
    PROGRAM,

    TEOF,

};
