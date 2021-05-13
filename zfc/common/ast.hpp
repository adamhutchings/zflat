/**
 * AST - the parent class of all syntax components.
 */

#pragma once

#include <fstream>
#include <vector>
#include <string>

#include <common/flow.hpp>
#include <common/operator.hpp>
#include <front/symbol.hpp>
#include <front/trfrags.hpp>

// Use this if an unexpected token pops up.
#define ZF_TOK_ERR(tok, exp_name) ZF_ERROR("expected %s on line %d, found '%s' instead", exp_name, tok.line, tok.raw_content())

std::string get_ident_name(std::ifstream& file);

class ASTNode {

public:

    // Construct this instance from a file, andd write this to a file
    virtual void read(std::ifstream& file) = 0;
    virtual void write(std::ofstream& file) = 0;

    // Apply a given function to all nodes and children.
    virtual void apply( void (*fn)(ASTNode*) ) = 0;

    virtual ~ASTNode() {}

    // Possibly - use for debug?
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
struct InnerExprNode;
struct FuncCallNode;
struct ControlFlowNode;
struct VarDeclNode;
struct DebugNode;
struct EnumDeclNode;
struct UseNode;

struct ProgramNode : public ASTNode {
    std::vector<UseNode*> imports; // All import statements - unused
    std::vector<ProgramSub*> components; // Each top-level component of the program aside from imports
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~ProgramNode();
};

struct ProgramSub : public ASTNode {
    virtual ~ProgramSub() {}
};

struct FunctionNode : public ProgramSub {
    sym::Function* symbol; // This is constructed by the read function.
    BlockStatementNode* body; // The body of the function
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~FunctionNode() {}
};

/**
 * This mess needs some explaining - when parsing a statement node, you can't
 * do this:
 * if (going_to_be_an_expr()) { this = new ExprNode(); }
 * Instead, each statement "subclass" is actually a subclass of InnerStatementNode
 * and one can do this->inner = new ExprNode();
 * TODO - Refactor
 */

struct InnerStatementNode : public ASTNode {
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
    // If we have a loop(x): y z();
    ExprNode* expr; // this is x
    StatementNode* stmt; // this is z
    VarDeclNode* pred; // this is y
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~LoopNode() {}
};

struct IfNode : public InnerStatementNode {
    ExprNode* expr; // the if condition
    StatementNode* stmt;
    StatementNode* else_block; // may be nullptr
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~IfNode() {}
};

struct SwitchNode : public InnerStatementNode {
    bool fswitch; // whether this switch statement uses the "fswitch" construct
    ExprNode* expr;
    std::vector<CaseNode*> cases;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~SwitchNode() {}
};

struct CaseNode {
    std::string lit; // literal value (TODO)
    std::vector<StatementNode*> statements;
    void read(std::ifstream& file);
    void write(std::ofstream& file);
    void apply( void (*fn)(ASTNode*) );
    virtual ~CaseNode() {}
};

struct InnerExprNode : public ASTNode {
    virtual ~InnerExprNode() {}
};

struct ExprNode : public InnerStatementNode {
    InnerExprNode* inner;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    inline void apply( void (*fn)(ASTNode*) ) override { fn(this->inner); fn(this); }
    virtual ~ExprNode() {}
};

struct BinaryExprNode : public InnerExprNode {
    ExprNode *left, *right;
    op::Operator op;
    inline void read(std::ifstream& file) override {}
    void write(std::ofstream& file) override;
    inline void apply( void (*fn)(ASTNode*) ) override { fn(this->left); fn(this->right); fn(this); }
    virtual ~BinaryExprNode() {}
};

struct VariableNode : public InnerExprNode {
    sym::Variable* sym;
    bool ref = false; // whether "ref x" or simply "x"
    inline void read(std::ifstream& file) override {}
    void write(std::ofstream& file) override;
    inline void apply( void (*fn)(ASTNode*) ) override { fn(this); }
    virtual ~VariableNode() {}
};

struct LiteralNode : public InnerExprNode {
    std::string lit;
    inline void read(std::ifstream& file) override {}
    void write(std::ofstream& file) override;
    inline void apply( void (*fn)(ASTNode*) ) override { fn(this); }
    virtual ~LiteralNode() {}
};

struct FuncCallNode : public InnerExprNode {
    std::string canonicalname; // the name assigned and then looked up later
    std::vector<ExprNode*> args;
    sym::Function* call;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    inline void apply( void (*fn)(ASTNode*) ) override { for (auto a : this->args) fn(a); fn(this); }
    virtual ~FuncCallNode() {}
};

struct ControlFlowNode : public InnerStatementNode {
    ControlFlow statement;
    ExprNode* expression; // Used only in return (as in return x + y;)
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~ControlFlowNode() {}
};

struct VarDeclNode : public InnerStatementNode {
    sym::Variable* var; // Constructed in the read method
    ExprNode* expr; // the "x + y" in "a: int = x + y;"
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~VarDeclNode() {}
};

struct DebugNode : public InnerStatementNode {
    int debug_val;
    BlockStatementNode* block;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~DebugNode() {}
};

struct EnumDeclNode : public ProgramSub {
    Enum* sym;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~EnumDeclNode() {}
};

struct UseNode : public ASTNode {
    std::vector<sym::Symbol*> symtab; // All of the symbols that have been imported
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    void apply( void (*fn)(ASTNode*) ) override;
    virtual ~UseNode() {}
};

/**
 * Until further refactoring takes place, this is necessary because there's no
 * way for an expression to "know" if it's a top level declaration, e.g. if
 * "2" is "2;" or "x = 2;".
 */
void warn_unused_value(ProgramSub* ps);
