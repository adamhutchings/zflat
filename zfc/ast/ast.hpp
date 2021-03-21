/**
 * AST - the parent class of all syntax components.
 */

#pragma once

#include <fstream>
#include <vector>
#include <string>

#include <front/trfrags.hpp>

#define ZF_TOK_ERR(tok, exp_name) ZF_ERROR("expected " exp_name " on line %d, found \"%s\" instead", tok.line, tok.raw_content())

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
    std::vector<ProgramSub*> components;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct ProgramSub : public ASTNode {};

struct FunctionNode : public ProgramSub {
    std::string name;
    std::vector<VarDeclNode> args;
    std::string ret_type;
    BlockStatementNode* body;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct InnerStatementNode : public ProgramSub {
    TreeComp type() override;
};

struct StatementNode : public ProgramSub {
    InnerStatementNode* inner;
    TreeComp type() override;
    void read(std::ifstream& file) override; // DO NOT USE
};

struct BlockStatementNode : public InnerStatementNode {
    std::vector<StatementNode*> statements;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct ExprNode : public InnerStatementNode {
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

struct ControlFlowNode : public InnerStatementNode {
    std::string statement;
    ExprNode* expression;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};

struct VarDeclNode : public InnerStatementNode {
    std::string ntype;
    std::string name;
    ExprNode* expr;
    TreeComp type() override;
    void read(std::ifstream& file) override;
};
