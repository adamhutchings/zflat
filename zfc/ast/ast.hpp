/**
 * AST - the parent class of all syntax components.
 */

#pragma once

#include <fstream>
#include <vector>
#include <string>

#include <back/flow.hpp>
#include <back/operator.hpp>
#include <front/symbol.hpp>
#include <front/trfrags.hpp>

#define ZF_TOK_ERR(tok, exp_name) ZF_ERROR("expected " exp_name " on line %d, found \"%s\" instead", tok.line, tok.raw_content())

class ASTNode {

public:

    virtual void read(std::ifstream& file) = 0;
    virtual void write(std::ofstream& file) = 0;

    // Apply a given function to all nodes and children.
    virtual void apply( void (*fn)(ASTNode*) ) = 0;

    virtual ~ASTNode() {}

    int line;

};

struct ProgramNode;
struct ProgramSub;
struct FunctionNode;
struct StatementNode;
struct BlockStatementNode;
struct LoopNode;
struct IfNode;
struct SwitchNode;
struct CaseNode;
struct ExprNode;
struct FuncCallNode;
struct ControlFlowNode;
struct VarDeclNode;
struct UseNode;

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
    sym::Function* symbol;
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

struct SwitchNode : public InnerStatementNode {
    bool fswitch;
    ExprNode* expr;
    std::vector<CaseNode*> cases;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
};

struct CaseNode {
    std::string lit; // literal value (TODO)
    std::vector<StatementNode*> statements;
    void read(std::ifstream& file);
    void write(std::ofstream& file);
    void apply( void (*fn)(ASTNode*) );
};

struct ExprNode : public InnerStatementNode {
    std::string literal; // number, perhaps
    sym::Symbol* ref;    // variable reference
    ExprNode* left;
    op::Operator op;
    ExprNode* right;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    void reorder();
    bool locked; // whether the expr should not be reordered (although its children may be)
    virtual ~ExprNode() {}
};

struct FuncCallNode : public ExprNode {
    std::vector<ExprNode*> args;
    sym::Function* call;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~FuncCallNode() {}
};

struct ControlFlowNode : public InnerStatementNode {
    ControlFlow statement;
    ExprNode* expression;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~ControlFlowNode() {}
};

struct VarDeclNode : public InnerStatementNode {
    sym::Variable* var;
    ExprNode* expr;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~VarDeclNode() {}
};

struct UseNode : public ASTNode {
    std::vector<sym::Symbol*> symtab;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~UseNode() {}
};

void warn_unused_value(ProgramSub* ps);
