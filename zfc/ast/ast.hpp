/**
 * AST - the parent class of all syntax components.
 */

#pragma once

#include <vector>
#include <string>

#include <front/trfrags.hpp>

class ASTNode {

public:

    virtual TreeComp type() = 0;

};

struct ProgramNode;
struct ProgramSub;
struct FunctionNode;
struct StatementNode;
struct BlockStatementNode;
struct ExprNode;
struct FuncCallNode;
struct ControlFlowNode;
struct VarDeclNode;

struct ProgramNode : public ASTNode {
    std::vector<ProgramSub> components;
    TreeComp type() override;
};

struct ProgramSub : public ASTNode {};

struct FunctionNode : public ProgramSub {
    std::vector<VarDeclNode> args;
    std::string retType;
    BlockStatementNode* body;
    TreeComp type() override;
};

struct StatementNode : public ProgramSub {
    TreeComp type() override;
};

struct BlockStatementNode : public StatementNode {
    std::vector<StatementNode> statements;
    TreeComp type() override;
};

struct ExprNode : public StatementNode {
    std::string literal;
    ExprNode* left;
    std::string op;
    ExprNode* right;
    TreeComp type() override;
};

struct FuncCallNode : public ExprNode {
    std::vector<ExprNode> args;
    TreeComp type() override;
};

struct ControlFlowNode : public StatementNode {
    std::string statement;
    ExprNode expression;
    TreeComp type() override;
};

struct VarDeclNode : public StatementNode {
    std::string ntype;
    std::string name;
    ExprNode* expr;
    TreeComp type() override;
};
