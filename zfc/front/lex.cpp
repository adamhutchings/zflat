#include "lex.hpp"

Token::Token() {
    
}

Token::Token(TreeComp t, std::string s)
: type(t), str(s) {

}

inline const char* Token::raw_content() {
    return this->str.c_str();
}

namespace {

// Whether in the INSIDE of a comment or a quote.
bool in_comment = false;

bool is_whitespace(char c) {
    switch (c) {
        case '\t':
        case ' ':
        case '\n':
            return true;
        default:
            return false;
    }
}

bool is_alpha(char c) {
    return ( (c >= 'a') && (c <= 'z') ) || ( (c >= 'A') && (c <= 'Z') );
}

bool is_numeric(char c) {
    return (c >= '0') && (c <= '9');
}

bool is_op_token(char c) {
    switch (c) {
        case '!':
        case '%':
        case '^':
        case '&':
        case '*':
        case '-':
        case '+':
        case '|':
        case '/':
        case '<':
        case '>':
        case '=':
            return true;
        default:
            return false;
    }
}

bool is_always_tok(char c) {
    switch (c) {
        case ':':
        case ';':
        case '[':
        case ']':
        case '{':
        case '}':
        case '(':
        case ')':
        case ',':
            return true;
        default:
            return false;
    }
}

const int TOK_MAX = 256;

char cbuf[TOK_MAX + 1];
int cend = 0;

void add_char(char c) {
    cbuf[cend++] = c;
}

bool begins_token(char prev, char cur) {
    if (cur == '~')                 return (!in_comment);
    if (in_comment)                 return false;
    if (is_whitespace(cur))         return false;
    if (is_whitespace(prev))        return true;
    if (cend == 0)                  return true;
    if (prev == '\0')               return true;
    if (is_op_token(cur))           return true;
    if (is_always_tok(cur))         return true;
    if (is_always_tok(prev))         return true;
    return false;
}

bool ends_token(char cur, char next) {
    if (cur == '~')                 return in_comment;
    if (in_comment)                 return false;
    if (is_whitespace(cur))         return false;
    if (is_whitespace(next))        return true;
    if (next < 1)                   return true;
    if (is_op_token(cur))           return (next != '=');
    if (is_always_tok(cur))         return true;
    if (is_always_tok(next))        return true;
    return false;
}

char lasttok = 0, curtok = 0, nexttok = 0;

bool eofhit = false;

Token process_character(std::ifstream& file) {
    if (eofhit) return Token(TreeComp::TEOF, "");
    lasttok = curtok;
    curtok = file.get();
    nexttok = file.peek();
    if (cend >= TOK_MAX)
        return Token(TreeComp::ETOOLARGE, cbuf);
    if (ends_token(curtok, nexttok)) {
        add_char(curtok);
        Token tok(TreeComp::DOT, cbuf);
        cend = 0;
        memset(cbuf, 0, sizeof(char) * TOK_MAX + 1);
        if (curtok == EOF) {
            tok.type = TreeComp::TEOF;
            eofhit = true;
        } else if (curtok == '~') {
            tok.type = TreeComp::COMMENT;
            in_comment = false;
        }
        return tok;
    } else {
        if (begins_token(lasttok, curtok)) {
            cend = 0;
        }
        if (curtok == '~') {
            in_comment = !in_comment;
        }
        if (!is_whitespace(curtok)) {
            add_char(curtok);
        }
    }
    return Token(TreeComp::EGENERALERROR, "");
}

Token inner_get_token(std::ifstream& file) {
    Token ret;
    while ( (ret = process_character(file)).type == TreeComp::EGENERALERROR );
    return ret;
}

}

Token lex::lex(std::ifstream& file) {
    Token tok;
    while ( (tok = inner_get_token(file)).type == TreeComp::COMMENT );
    return tok;
}
