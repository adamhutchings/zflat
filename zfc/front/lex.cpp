#include "lex.hpp"

Token::Token(TreeComp t, std::string s)
: type(t), str(s) {

}

inline const char* Token::raw_content() {
    return this->str.c_str();
}
