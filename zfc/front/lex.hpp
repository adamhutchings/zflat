/**
 * lex - get tokens.
 */

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <string.h>

#include <front/trfrags.hpp>

struct Token {

    TreeComp type;
    std::string str;
    int line;
    int linepos;

    inline const char* raw_content();
    Token(TreeComp t, std::string s);
    Token();

};

namespace lex {
    Token lex(std::ifstream& file);
}
