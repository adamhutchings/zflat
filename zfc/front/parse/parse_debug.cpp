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
            int val;
            try {
                int val = std::stoi(lit.str);
            } catch (std::invalid_argument a) {
                // Not a valid literal!
                ZF_TOK_ERR(lit, "a valid integer literal");
            }
            // Alright, make sure it's in range.
            if (val > debug::level_max) {
                ZF_TOK_ERR(lit, "a value in the range for debug ranges");
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
