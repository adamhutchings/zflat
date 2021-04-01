#include "ast.hpp"

namespace {
void pdel(ASTNode* node) { delete node; }
}

ProgramNode::~ProgramNode() {
    this->apply(pdel);
}

void ProgramNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    for (auto sub : this->components) {
        fn(sub);
    }
}

void FuncCallNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    for (auto arg : this->args) {
        fn(arg);
    }
}

void FunctionNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    for (auto decl : this->args) {
        fn(decl);
    }
    fn(this->body);
}

void StatementNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    fn(this->inner);
}

void BlockStatementNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    for (auto stmt : this->statements) {
        fn(stmt);
    }
}

void IfNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    fn(this->expr);
    fn(this->stmt);
    if (this->else_block != nullptr) delete this->else_block;
}

void LoopNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    fn(this->expr);
    fn(this->pred);
    fn(this->stmt);
}

void ExprNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    if (this->left != nullptr) fn(this->left);
    if (this->right != nullptr) fn(this->left);
}

void ControlFlowNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    if (this->expression != nullptr) fn(this->expression);
}

void VarDeclNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
    if (this->expr != nullptr) fn(this->expr);
}
