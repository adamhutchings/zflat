#include "ast.hpp"

TreeComp ProgramNode::type() {
    return TreeComp::PROGRAM;
}

TreeComp FunctionNode::type() {
    return TreeComp::FUNCTION_DECL;
}

TreeComp StatementNode::type() {
    return TreeComp::STATEMENT;
}

TreeComp BlockStatementNode::type() {
    return TreeComp::BLOCK_STATEMENT;
}

TreeComp ExprNode::type() {
    return TreeComp::EXPRESSION;
}

TreeComp ControlFlowNode::type() {
    return TreeComp::FLOW_STATEMENT;
}

TreeComp VarDeclNode::type() {
    return TreeComp::VAR_DECL;
}
