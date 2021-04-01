#include "operator.hpp"

namespace op {

int binding_order(Operator a, Operator b) {
    int diff = (a / 8) - (b - 8);
    return (diff == 0) ? 0 : (diff < 0) ? -1 : 1;
}

Operator strToOp(std::string in) {
    if (in == "=") return EQUALS;
    if (in == "+=") return ADDEQ;
    if (in == "-=") return SUBEQ;
    if (in == "*=") return MULEQ;
    if (in == "/=") return DIVEQ;
    if (in == "%=") return REMEQ;
    if (in == "&=") return ANDEQ;
    if (in == "|=") return OREQ;
    if (in == "^=") return XOREQ;
    if (in == "==") return EQ;
    if (in == "!=") return NEQ;
    if (in == ">") return GREATER;
    if (in == "<") return LESSER;
    if (in == ">=") return GREQ;
    if (in == "<=") return LESSEQ;
    if (in == "+") return ADD;
    if (in == "-") return SUB;
    if (in == "*") return MUL;
    if (in == "/") return DIV;
    if (in == "%") return REM;
    if (in == "&") return AND;
    if (in == "|") return OR;
    if (in == "^") return XOR;
}

}

// Reorder an expr to follow order of operations
void ExprNode::reorder() {

    // Check the right node first, if it needs to be reordered.
    // Check: right side exists, right side has an operator
    if (this->right != nullptr && this->right->right != nullptr
    // op binds more tightly than right side op
    && op::binding_order(op::strToOp(this->op), op::strToOp(this->right->op)) < 0
    // Make sure the right side is not locked
    && !this->right->locked
    ) {
        // Structure right now is A * (B + C)
        // Change it to (A * B) + C
        ExprNode* ltemp = this->left;
        this->left = new ExprNode();
        this->left->left = ltemp;
        this->left->op = this->op;
        this->left->right = this->right->left;
        this->op = this->right->op;
        this->right = this->right->right;
        this->right->reorder();
    }

    // Same check for left
    // Check the left node first, if it needs to be reordered.
    // Check: left side exists, left side has an operator
    if (this->left != nullptr && this->left->right != nullptr
    // op binds more tightly than left side op
    && op::binding_order(op::strToOp(this->op), op::strToOp(this->left->op)) < 0
    // Make sure the left side is not locked
    && !this->left->locked
    ) {
        // Structure right now is (A + B) * C
        // Change it to A + (B * C)
        ExprNode* rtemp = this->right;
        this->right = new ExprNode();
        this->right->right = rtemp;
        this->right->op = this->op;
        this->right->left = this->left->right;
        this->op = this->left->op;
        this->left = this->left->left;
        this->left->reorder();
    }

}

void g_reorder(ASTNode* node) {
    ExprNode* xp;
    if ( (xp = dynamic_cast<ExprNode*> (node)) != nullptr )
        xp->reorder();
}

void reorder(ProgramNode* pn) {
    pn->apply(g_reorder);
}
