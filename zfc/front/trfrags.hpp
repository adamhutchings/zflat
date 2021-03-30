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
