#include "lex.hpp"

Token::Token() {
    
}

Token::Token(TreeComp t, std::string s)
: type(t), str(s) {

}

const char* Token::raw_content() {
    return this->str.c_str();
}

namespace {

// These are misnomers - they refer to the last, present, and next characters.
char lasttok = 0, curtok = 0, nexttok = 0;

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

// This token is an operator
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

// Whereas operators can be part of another token (as in +=, etc.), these tokens
// always stand on their own.
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
        case '.':
            return true;
        default:
            return false;
    }
}

// The maximum langth of a token
const int TOK_MAX = 256;

// As characters are read from the file, they get placed into here - when the
// end of a token is reached, all characters are popped into a string and
// returned.
char cbuf[TOK_MAX + 1];

// This is the character PAST the last character in the token.
int cend = 0;

// eofhit - whether the end-of-file is currently detected.
// lastwascolon - if the last token to be lexed was a colon.
// whether we are currently in a quote.
bool eofhit = false, lastwascolon = false, in_quote = false;

void add_char(char c) {
    cbuf[cend++] = c;
}

// Set the end to 0 and clear the buffer
void reset_buf_state() {
    cend = 0;
    if (cbuf[0] == '\"') in_quote = false;
    memset(cbuf, 0, sizeof(char) * TOK_MAX + 1);
}

int line = 1, linepos = 1;

// How many backslashes in a row have been encountered.
// Right now, parsing string literals is very unstable - it takes in all the
// characters and does not attempt to check, only assuming that C string literal
// notation is the same as its own. Which it is. For now.
int bsct = 0;

bool begins_token(char prev, char cur) {
    if (cur == '~')                 return (!in_comment);
    if (cur == '\"')                return !in_quote;
    if (in_quote)                   return false;
    if (in_comment)                 return false;
    if (is_whitespace(cur))         return false;
    if (is_whitespace(prev))        return true;
    if (cend == 0)                  return true;
    if (prev == '\0')               return true;
    if (is_op_token(cur))           return true;
    if (is_always_tok(cur))         return true;
    if (is_always_tok(prev))        return true;
    return false;
}

bool ends_token(char cur, char next) {
    if (cur == '~')                 return in_comment;
    if (cur == '\"')                return (bsct / 2) == 0 && cend > 1;
    if (in_quote)                   return false;
    if (in_comment)                 return false;
    if (is_whitespace(cur))         return false;
    if (is_whitespace(next))        return true;
    if (next < 1)                   return true;
    if (is_op_token(cur))           return (next != '=');
    if (is_always_tok(cur))         return true;
    if (is_always_tok(next))        return true;
    return false;
}

TreeComp get_keyword_type(std::string name) {
    if (name == "loop")          return TreeComp::LOOP;
    if (name == "if")            return TreeComp::IF;
    if (name == "else")          return TreeComp::ELSE;
    if (name == "switch")        return TreeComp::SWITCH;
    if (name == "case")          return TreeComp::CASE;
    if (name == "fswitch")       return TreeComp::FSWITCH;
    if (name == "extc")          return TreeComp::EXTC;
    if (name == "__c_va_args")   return TreeComp::VA_ARGS;
    if (name == "use")           return TreeComp::USE;
    if (name == "ref")           return TreeComp::REF;
    if (name == "enum")          return TreeComp::ENUM;
    if (name == "bitfield")      return TreeComp::BITFIELD;
    return TreeComp::EGENERALERROR;
}

bool is_builtin_type(std::string name) {
    for (int typei = 0; typei < static_cast<int>(BuiltinType::MAX_INVALID); ++typei) {
        auto type = static_cast<BuiltinType>(typei);
        if (typeToZStr(type) == name) return true;
    }
    return false;
}

TreeComp get_type(std::string name) {
    if (curtok == EOF) return TreeComp::TEOF;
    switch (curtok) {
        case '~':
            return TreeComp::COMMENT;   
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
            return TreeComp::OPERATOR;
        case ':':
            return TreeComp::COLON;
        case ';':
            return TreeComp::SEMICOLON;
        case '[':
            return TreeComp::OBRACKET;
        case ']':
            return TreeComp::CBRACKET;
        case '{':
            return TreeComp::OBRACE;
        case '}':
            return TreeComp::CBRACE;
        case '(':
            return TreeComp::OPAREN;
        case ')':
            return TreeComp::CPAREN;
        case ',':
            return TreeComp::COMMA;
        case '.':
            return TreeComp::DOT;
        default:;
    }
    if (name == "break" || name == "continue" || name == "return")
        return TreeComp::FLOW;
    if (is_builtin_type(name)) return TreeComp::TYPENAME;
    auto kwt = get_keyword_type(name);
    if (kwt != EGENERALERROR) return kwt;
    if (is_alpha(name[0])) {
        return (lastwascolon) ? TreeComp::TYPENAME : TreeComp::IDENTIFIER;
    }
    if (is_numeric(name[0]) || name[0] == '\'' || name[0] == '\"')
        return TreeComp::LITERAL;
    // when in doubt ...
    ZF_ERROR("invalid material for token: on line %d, found %s", line, name.c_str());
}

// Edit the backslash count for quote parsing according to the current state.
inline void manage_backslashes() {
    if (curtok == '\\') {
        ++bsct;
    } else {
        bsct = 0;
    }
}

// Edit the line and linepos counts according to the current state.
inline void manage_line_and_linepos() {
    if (curtok == '\n') {
        ++line;
        linepos = 1;
    } else {
        ++linepos;
    }
}

// Move the character set one position ahead in the file.
inline void advance_characters(std::ifstream& file) {
    lasttok = curtok;
    curtok = file.get();
    nexttok = file.peek();
}

inline void update_eofflag(std::ifstream& file) {
    if (file.peek() == EOF)
        eofhit = true;
}

inline void reset_token_flags() {
    in_comment = false;
    in_quote = false;
}

inline void update_comment_flags() { 
    if (curtok == '~') {
        in_comment = true;
    }
}

// Whether a character should be added to the buffer or ignored.
inline bool yes_add_char(char c) {
    return (!is_whitespace(c) && !in_comment) || in_quote;
}

// A put-back token lands in here.
// The next call to lex::lex will scan this buffer first.
std::vector<Token> put_back_tokens;

Token process_character(std::ifstream& file) {
    if (eofhit) {
        if (in_comment) ZF_ERROR("Open comment");
        if (in_quote) ZF_ERROR("Open quote");
        // Return this every time and don't bother going through the motions.
        return Token(TreeComp::TEOF, "[end of file]");
    }
    advance_characters(file);
    manage_line_and_linepos();
    if (curtok == '\"' && !in_quote) {
        in_quote = true;
    }
    if (cend >= TOK_MAX)
        ZF_ERROR("token on line %u exceeded limit", line);
    update_eofflag(file);
    if (begins_token(lasttok, curtok)) {
        // Clearing the buffer state - this should theoretically be
        // unnecessary, but do it anyway in case an edge case comes up.
        reset_buf_state();
    }
    if (yes_add_char(curtok)) {
        add_char(curtok);
    }
    if (ends_token(curtok, nexttok)) {
        // Clear the buffer, but first create a token and return it.
        Token tok(get_type(cbuf), cbuf);
        lastwascolon = curtok == ':';
        reset_token_flags();
        return tok;
    }
    update_comment_flags();
    manage_backslashes();
    // Otherwise, return an invalid token.
    return Token(TreeComp::EGENERALERROR, "");
}

Token inner_get_token(std::ifstream& file) {
    // Go over characters one at a time until the routine returns a valid token.
    Token ret;
    while ( (ret = process_character(file)).type == TreeComp::EGENERALERROR );
    ret.line = line;
    ret.linepos = linepos;
    return ret;
}

}

Token lex::lex(std::ifstream& file) {
    Token tok;
    // Scan put-back buffer first.
    if (put_back_tokens.size() > 0) {
        tok = put_back_tokens[put_back_tokens.size() - 1];
        put_back_tokens.pop_back();
    }
    // Don't bother with comments. This was a hacky fix from a while ago.
    else
        while ( (tok = inner_get_token(file)).type == TreeComp::COMMENT );
    return tok;
}

void lex::unlex(Token token) {
    put_back_tokens.push_back(token);
}

void lex::reset_lexer() {
    line = 1;
    linepos = 1;
    put_back_tokens.clear();
    reset_buf_state();
    reset_token_flags();
    eofhit = false;
}
