#include "ast.hpp"

TreeComp ProgramNode::type() {
    return TreeComp::PROGRAM;
}

ProgramNode::~ProgramNode() {
    for (auto sub : this->components)
        delete sub;
}

ProgramSub::~ProgramSub() {}

TreeComp FunctionNode::type() {
    return TreeComp::FUNCTION_DECL;
}

FunctionNode::~FunctionNode() {
    for (auto decl : this->args) {
        delete decl;
    }
    delete this->body;
}

TreeComp InnerStatementNode::type() {
    return TreeComp::STATEMENT;
}

TreeComp StatementNode::type() {
    return TreeComp::STATEMENT;
}

StatementNode::~StatementNode() {
    delete this->inner;
}

TreeComp BlockStatementNode::type() {
    return TreeComp::BLOCK_STATEMENT;
}

BlockStatementNode::~BlockStatementNode() {
    for (auto stmt : this->statements) {
        delete stmt;
    }
}

TreeComp ExprNode::type() {
    return TreeComp::EXPRESSION;
}

ExprNode::~ExprNode() {
    if (this->left != nullptr) delete this->left;
    if (this->right != nullptr) delete this->right;
}

TreeComp ControlFlowNode::type() {
    return TreeComp::FLOW_STATEMENT;
}

ControlFlowNode::~ControlFlowNode() {
    if (this->expression != nullptr) delete this->expression;
}

TreeComp VarDeclNode::type() {
    return TreeComp::VAR_DECL;
}

VarDeclNode::~VarDeclNode() {
    if (this->expr != nullptr) delete this->expr;
}
