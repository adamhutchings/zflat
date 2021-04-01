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

    virtual void read(std::ifstream& file) = 0;
    virtual void write(std::ofstream& file) = 0;

    // Apply a given function to all nodes and children.
    virtual void apply( void (*fn)(ASTNode*) ) = 0;

    virtual ~ASTNode() {}

};

struct ProgramNode;
struct ProgramSub;
struct FunctionNode;
struct StatementNode;
struct BlockStatementNode;
struct LoopNode;
struct ExprNode;
struct FuncCallNode;
struct ControlFlowNode;
struct VarDeclNode;

struct ProgramNode : public ASTNode {
    std::vector<ProgramSub*> components;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~ProgramNode();
};

struct ProgramSub : public ASTNode {
    virtual ~ProgramSub() {}
};

struct FunctionNode : public ProgramSub {
    std::string name;
    std::vector<VarDeclNode*> args;
    std::string ret_type;
    BlockStatementNode* body;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~FunctionNode() {}
};

struct InnerStatementNode : public ProgramSub {
    virtual ~InnerStatementNode() {}
};

struct StatementNode : public ProgramSub {
    InnerStatementNode* inner;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~StatementNode() {}
};

struct BlockStatementNode : public InnerStatementNode {
    std::vector<StatementNode*> statements;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~BlockStatementNode() {}
};

struct LoopNode : public InnerStatementNode {
    ExprNode* expr;
    StatementNode* stmt;
    VarDeclNode* pred; // the loop counter variable, if any
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~LoopNode() {}
};

struct IfNode : public InnerStatementNode {
    ExprNode* expr;
    StatementNode* stmt;
    StatementNode* else_block; // may be nullptr
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~IfNode() {}
};

struct ExprNode : public InnerStatementNode {
    std::string literal;
    ExprNode* left;
    std::string op;
    ExprNode* right;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    std::string get_type();
    void reorder();
    bool locked; // whether the expr should not be reordered (although its children may be)
    virtual ~ExprNode() {}
};

struct FuncCallNode : public ExprNode {
    std::vector<ExprNode*> args;
    std::string name;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~FuncCallNode() {}
};

struct ControlFlowNode : public InnerStatementNode {
    std::string statement;
    ExprNode* expression;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~ControlFlowNode() {}
};

struct VarDeclNode : public InnerStatementNode {
    std::string ntype;
    std::string name;
    ExprNode* expr;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~VarDeclNode() {}
};
