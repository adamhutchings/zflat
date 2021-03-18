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

    // Tree fragments
    EXPRESSION,
    BARE_VAR_DECL, // x: int
    FULL_VAR_DECL, // x: int = 3
    FLOW_STATEMENT, // break, return, continue
    STATEMENT,
    BLOCK_STATEMENT,
    FUNCTION_DECL,
    USE_STATEMENT,
    PROGRAM,

};
