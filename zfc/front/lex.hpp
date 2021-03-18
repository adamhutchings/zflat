/**
 * lex - get tokens.
 */

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <front/trfrags.hpp>

struct Token {

    TreeComp type;
    std::string str;
    int line;
    int linepos;

    inline const char* raw_content();
    Token(TreeComp t, std::string s);

};

namespace lex {
    Token lex(std::ifstream& file);
}
