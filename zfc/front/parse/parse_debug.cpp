#include <common/debug.hpp>
#include <front/parse/parse_header.hpp>

void DebugNode::read(std::ifstream& file) {

    this->line = expect(file, DEBUG).line;

    auto nx = lex::lex(file);
    Token lit;

    switch (nx.type) {
        case TreeComp::OPAREN:
            // Parse the value after
            lit = expect(file, LITERAL);
            try {
                int val = std::stoi(lit.str);
                // Alright, make sure it's in range.
                if (val > debug::level_max) {
                    ZF_TOK_ERR(lit, "a value in the range for debug ranges");
                }
                this->debug_val = val;
            } catch (std::invalid_argument a) {
                // Not a valid literal!
                ZF_TOK_ERR(lit, "a valid integer literal");
            }
            expect(file, CPAREN);
            // Fall-through to the obrace statement. Assigning to nx so the
            // lex::unlex call unlexes the right token.
            nx = expect(file, OBRACE);
        case TreeComp::OBRACE:
            // Parse the debug statement
            this->debug_val = debug::level_default;
            lex::unlex(nx);
            this->block = new BlockStatementNode();
            this->block->read(file);
            break;
        default:
            ZF_TOK_ERR(nx, "'{' or '('");
    }

}
