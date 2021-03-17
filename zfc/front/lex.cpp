#include "lex.hpp"

namespace {

    bool is_whitespace(char c) {
        switch (c) {
            case ' ':
            case '\t':
            case '\n':
                return true;
            default:
                return false;
        }
    }

    bool is_numeric(char c) {
        return ( (c >= '0') && (c <= '9') );
    }

    bool is_alpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    std::string get_tok(std::ifstream& file, int begin, int end) {
        std::string tokct;
        file.seekg(begin);
        for (int i = 0; i < end - begin - 1; i++) {
            tokct += file.get();
        }
        return tokct;
    }

    bool last_returned_token_was_colon = false; // for determining types

}

int tokens = 0;

Token& lex(std::ifstream& file, Token& tok) {

    // Skip whitespace and comments
    while (is_whitespace(file.peek())) {
        file.get();
    }

    char n = file.get();

    if (n == '~') {
        do {
            n = file.get();
            if (n == EOF) {
                throw "Open comment";
            }
        } while (n != '~');
        // now we're at the last '~', so go forward one.
        file.get();
    }

    // the pointer is now at the very first non-whitespace character.
    int c = file.peek();

    if (c == EOF) {
        tok.type = TEOF;
        goto end;
    }

    // Check quotes.
    if ( (c == '\"') || (c == '\'') ) {

        // We have to parse a quote, so preserve the position of this so we can
        // seek back and read the whole token.
        int beginpos = file.tellg();

        // A quote only ends the literal if it is preceded by an even number of
        // backslashes.
        int bct = 0;

        char sub;

        do {
            sub = file.get();
            // the quote didn't end.
            if (sub == EOF) {
                throw "Open quote";
            }
            if (sub == '\\') ++bct; else bct = 0;
        } while ( (sub != c) && (bct % 2 == 0) );

        tok.str = get_tok(file, beginpos, file.tellg());
        tok.type = TokenType::LITERAL;
        goto end;

    }

    // Numeric literal
    if (is_numeric(c)) {

        char sub;
        int beginpos = file.tellg();

        do {
            sub = file.peek();
        } while ( is_numeric(c) || (c == '.') );

        tok.str = get_tok(file, beginpos, file.tellg());
        tok.type = TokenType::LITERAL;
        goto end;

    }

    // Single characters
    switch (c) {
        case ':':
            tok.str = ':';
            tok.type = TokenType::COLON;
            last_returned_token_was_colon = true;
            ++tokens;
            return tok;
        case '(':
            tok.str = '(';
            tok.type = TokenType::OPAREN;
            goto end;
        case ')':
            tok.str = ')';
            tok.type = TokenType::CPAREN;
            goto end;
        case '{':
            tok.str = '{';
            tok.type = TokenType::OBRACE;
            goto end;
        case '}':
            tok.str = '}';
            tok.type = TokenType::CBRACE;
            goto end;
        case '[':
            tok.str = '[';
            tok.type = TokenType::OBRACKET;
            goto end;
        case ']':
            tok.str = ']';
            tok.type = TokenType::CBRACKET;
            goto end;
        case ';':
            tok.str = ';';
            tok.type = TokenType::SCOLON;
            goto end;
        default:;
    }

    // Now, we have an identifier, a typename, or an operator, or a control flow
    
    // Identifier or control flow
    if (is_alpha(c)) {

        char n = c;
        int beginpos = file.tellg();
        do {
            n = file.get();
        } while (is_alpha(n) || (n == '_') || is_numeric(n));
        tok.str = get_tok(file, beginpos, file.tellg());

        // Control flow check
        if (tok.str == "break" || tok.str == "return" || tok.str == "continue") {
            printf("registered %s\n", tok.str.c_str());
            tok.type = TokenType::CFLOW;
            goto end;
        } else if (last_returned_token_was_colon) {
            tok.type = TokenType::TYPENAME;
            goto end;
        } else {
            tok.type = TokenType::IDENTIFIER;
            goto end;
        }
        
    }

    end:
        ++tokens;
        if (
            (tok.type == TokenType::IDENTIFIER)
         || (tok.type == TokenType::TYPENAME)
        ) {
            file.unget();
        }
        last_returned_token_was_colon = false;
        return tok;

}
