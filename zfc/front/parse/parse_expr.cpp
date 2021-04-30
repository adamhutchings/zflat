#include <fstream>

#include <ast/ast.hpp>
#include <back/operator.hpp>
#include <front/lex.hpp>
#include <util/error.hpp>

namespace {

/**
 * Read a single expression, non-binary if non-parenthesized. It will return
 * nullptr if no more exprs remain.
 */
InnerExprNode* single_node_read(std::ifstream& file) {
    Token next = lex::lex(file);
    Token next2;
    std::string name;
    ExprNode* expr;
    InnerExprNode* iexpr;
    sym::Variable* vsym;
    bool refflag = false; // whether a potential variable is ref-qualified
    switch (next.type) {
        case OPAREN:
            expr = new ExprNode();
            expr->read(file);
            // Expect closing paren.
            next2 = lex::lex(file);
            if (next2.type != CPAREN) {
                ZF_TOK_ERR(next2, "')'");
            }
            iexpr = expr->inner;
            delete expr;
            return iexpr;
        case REF:
            refflag = true;
            // Lex the next token
            // It sshould be an identifier, so just go on to the next case
            next = lex::lex(file);
            if (next.type != IDENTIFIER) {
                ZF_TOK_ERR(next, "identifier");
            }
        case IDENTIFIER:
            lex::unlex(next);
            name = get_ident_name(file);
            next2 = lex::lex(file);
            if (next2.type == OPAREN) {
                iexpr = new FuncCallNode();
                dynamic_cast<FuncCallNode*>(iexpr)->canonicalname = name;
                lex::unlex(next2);
                iexpr->read(file);
                return iexpr;
            } else if (next2.type == OBRACKET) {
                // The expression is an array index.
                expr = new ExprNode();
                expr->read(file);
                // Expect closing bracket
                auto cb = lex::lex(file);
                if (cb.type != CBRACKET) {
                    ZF_TOK_ERR(cb, "']'");
                }
                iexpr = new BinaryExprNode();
                auto bexpr = dynamic_cast<BinaryExprNode*>(iexpr);
                bexpr->left = new ExprNode();
                bexpr->left->inner = new VariableNode();
                // This is slightly copy-pasted from below.
                // OK, 100% copy-pasted.
                vsym = sym::resolve_var(name);
                if (vsym == nullptr) {
                    ZF_ERROR("line %d: could not resolve variable %s", next.line, next.raw_content());
                }
                dynamic_cast<VariableNode*>(bexpr->left->inner)->sym = vsym;
                bexpr->op = op::Operator::INDEX;
                bexpr->right = expr;
                bexpr->line = next2.line;
                return iexpr;
            } else {
                lex::unlex(next2);
                iexpr = new VariableNode();
                vsym = sym::resolve_var(name);
                if (vsym == nullptr) {
                    ZF_ERROR("line %d: could not resolve variable %s", next.line, next.raw_content());
                }
                dynamic_cast<VariableNode*>(iexpr)->sym = vsym;
                dynamic_cast<VariableNode*>(iexpr)->ref = refflag;
                return iexpr;
            }
        case LITERAL:
            iexpr = new LiteralNode();
            dynamic_cast<LiteralNode*>(iexpr)->lit = next.str;
            return iexpr;
        default:
            lex::unlex(next);
            return nullptr;
    }
}

}

void ExprNode::read(std::ifstream& file) {

    /* Sketchy algorithm o-clock!
     * from Binkiklou, I crown this Adam's Sketchy Expression-Parsing Algorithm.
     * We read a vector of expression nodes and a vector of operators
     * between them. The nodes shouldn't be tree nodes, so we organize them into
     * a tree based on priority. However, if an expression is parenthesized, it
     * will be parsed as one expression.
     */

    std::vector<InnerExprNode*> nodes;
    std::vector<op::Operator> ops;

    int line;

    // BEGIN - read operators and expressions
    int nc = 0;
    while (1) {
        // Read an operator
        if (nc != 0) {
            auto opt = lex::lex(file);
            if (opt.type != OPERATOR) {
                lex::unlex(opt);
                break;
            }
            auto op = op::strToOp(opt.str);
            if (op == op::Operator::INVALID) {
                ZF_TOK_ERR(opt, "valid operator token");
            }
            ops.push_back(op);
            line = opt.line;
        }
        // Read one atomic expression.
        auto temp = single_node_read(file);
        if (temp == nullptr) break;
        nodes.push_back(temp);
        ++nc;
    }

    if (nodes.size() == 0) {
        ZF_ERROR("line %d: expression incomplete", line);
    }

    // Now, we combine over and over.
    while (nodes.size() > 1) {

        // Traverse to find highest-precedence operator in the list.
        int splice_loc = 0;
        int size = ops.size();
        for (int i = 0; i < size; ++i) {
            if (op::binding_order(ops[splice_loc], ops[i]) == -1) {
                splice_loc = i;
            }
        }

        // Now we combine the two nodes adjacent to splice_loc.
        InnerExprNode* treefrag = new BinaryExprNode();
        BinaryExprNode* tree = dynamic_cast<BinaryExprNode*>(treefrag);
        tree->left = new ExprNode();
        tree->right = new ExprNode();
        tree->left->inner = nodes[splice_loc];
        tree->right->inner = nodes[splice_loc + 1];
        tree->op = ops[splice_loc];

        // Remove the two old nodes from the list.
        // This is meant to be doubled.
        nodes.erase(nodes.begin() + splice_loc);
        nodes.erase(nodes.begin() + splice_loc);

        // Remove old op.
        ops.erase(ops.begin() + splice_loc);

        // Add new element to nodes list.
        nodes.insert(nodes.begin() + splice_loc, treefrag);

    }

    this->inner = nodes[0];

}
