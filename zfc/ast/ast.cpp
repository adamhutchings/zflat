#include "ast.hpp"

ProgramNode::~ProgramNode() {
    for (auto sub : this->components)
        delete sub;
}

ProgramSub::~ProgramSub() {}

FuncCallNode::~FuncCallNode() {
    for (auto arg : this->args) {
        delete arg;
    }
}

FunctionNode::~FunctionNode() {
    for (auto decl : this->args) {
        delete decl;
    }
    delete this->body;
}

InnerStatementNode::~InnerStatementNode() {}

StatementNode::~StatementNode() {
    delete this->inner;
}

BlockStatementNode::~BlockStatementNode() {
    for (auto stmt : this->statements) {
        delete stmt;
    }
}

IfNode::~IfNode() {
    delete this->expr;
    delete this->stmt;
}

LoopNode::~LoopNode() {
    delete this->expr;
    delete this->pred;
    delete this->stmt;
}

ExprNode::~ExprNode() {
    if (this->left != nullptr) delete this->left;
    if (this->right != nullptr) delete this->right;
}

ControlFlowNode::~ControlFlowNode() {
    if (this->expression != nullptr) delete this->expression;
}

VarDeclNode::~VarDeclNode() {
    if (this->expr != nullptr) delete this->expr;
}
