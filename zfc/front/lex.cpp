#include "lex.hpp"

Token::Token(TreeComp t, std::string s)
: type(t), str(s) {

}

inline const char* Token::raw_content() {
    return this->str.c_str();
}

namespace {

const int MAX_TOKEN_SIZE = 256;

bool is_whitespace(char c) {
    switch (c) {
        case '\t':
        case '\n':
        case ' ':
            return true;
        default:
            return false;
    }
}

bool is_numeric(char c) {
    return (c >= '0') && (c <= '9');
}

bool is_allowed_identifier_start(char c) {
    return ( (c >= 'a') && (c <= 'z') ) || ( (c >= 'A') && (c <= 'Z') )
    || (c == '_');
}

bool is_allowed_identifier_char(char c) {
    return is_allowed_identifier_start(c) || (is_numeric(c));
}

// Where the current token is being placed.
std::vector<char> tokenbuf(MAX_TOKEN_SIZE + 1);

// Whether we're in a comment.
bool in_comment = false;
bool in_quote = false;

int current_line = 1;
int current_pos = 0;
TreeComp curtype;

char lastchar;
int bscount; // how much BS I've seen - I mean, how many backslashes in a row
char qchar; // which character opened the quote

// Whether this is this character AFTER the end of a token
bool process_char(int c) {

    if (c == EOF) {
        return true;
    }

    if (c == '\n') {
        ++current_line;
        current_pos = 1;
    } else {
        ++current_pos;
    }

    if (c == '~') {
        in_comment = !in_comment;
        return false;
    }
    if (in_comment)
        return false;

    if (is_whitespace(c))
        if (!is_whitespace(lastchar))
            return !in_quote;
        else
            return false;

    // Check size of token
    if (tokenbuf.size() == MAX_TOKEN_SIZE) {
        throw "Token too large";
    }

    if (c == '"' || c == '\'') {
        if (!in_quote) {
            in_quote = true;
            qchar = c;
            curtype = TreeComp::LITERAL;
        } else if (c == qchar) {
            if (bscount % 2 == 0) {
                in_quote = false;
                goto end; // skip backslash counting
            }
        }
    }

    if (in_quote) {
        goto end;
    }

    if (is_allowed_identifier_start(c)) {
        if (is_allowed_identifier_start(lastchar) || is_numeric(c)) {
            goto end;
        } else {
            curtype = (lastchar == ':') ? TreeComp::TYPENAME : TreeComp::IDENTIFIER;
        }
    }

    // Unsupported characters
    switch (c) {
        case '@':
        case '#':
        case '$':
        case '[':
        case ']':
        case '\\':
        case '?':
            throw "Unsupported character";
        default:;
    }

    // Operators
    switch (lastchar) {
        case '*':
        case '/':
        case '+':
        case '-':
        case '%':
        case '&':
        case '^':
        case '|':
        case '!':
        case '<':
        case '>':
            return(c != '=');
        default:;
    }

    if (c == '\\') {
        ++bscount;
    } else {
        bscount = 0;
    }
    
end:
    tokenbuf.push_back(c);
    lastchar = c;

}

Token get_token(TreeComp t) {

    tokenbuf.push_back('\0');
    Token tok(t, std::string(tokenbuf.data()));
    tok.line = current_line;
    tok.linepos = current_pos;

    tokenbuf.clear();

    return tok;

}

}

namespace lex {

Token lex::lex(std::ifstream& file) {
    while(!process_char(file.get()));
    // Move one character back
    file.unget();
    lastchar = '\0';
    return get_token(curtype);
}

}
