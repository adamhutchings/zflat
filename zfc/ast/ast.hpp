/**
 * AST - the parent class of all syntax components.
 */

#pragma once

#include <fstream>
#include <vector>
#include <string>

#include <front/trfrags.hpp>

class ASTNode {

public:

    virtual TreeComp type() = 0;

    virtual void read(std::ifstream& file) = 0;

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
    void read(std::ifstream& file) override;
};

struct ProgramSub : public ASTNode {};

struct FunctionNode : public ProgramSub {
    std::vector<VarDeclNode> args;
    std::string retType;
    BlockStatementNode* body;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct StatementNode : public ProgramSub {
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct BlockStatementNode : public StatementNode {
    std::vector<StatementNode> statements;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct ExprNode : public StatementNode {
    std::string literal;
    ExprNode* left;
    std::string op;
    ExprNode* right;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct FuncCallNode : public ExprNode {
    std::vector<ExprNode> args;
    std::string name;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct ControlFlowNode : public StatementNode {
    std::string statement;
    ExprNode* expression;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct VarDeclNode : public StatementNode {
    std::string ntype;
    std::string name;
    ExprNode* expr;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};
