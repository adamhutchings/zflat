#include "ast.hpp"

TreeComp ProgramNode::type() {
    return TreeComp::PROGRAM;
}

ProgramNode::~ProgramNode() {
    for (auto sub : this->components)
        delete sub;
}

void ProgramNode::validate() {
    for (auto sub : this->components)
        sub->validate();
}

ProgramSub::~ProgramSub() {}

TreeComp FunctionNode::type() {
    return TreeComp::FUNCTION_DECL;
}

void FunctionNode::validate() {

    // We need to make a symbol table with the arguments
    auto* tab = new sym::SymbolTable();

    for (VarDeclNode* vdn : this->args) {
        auto* sym = new sym::Symbol();
        sym->name = vdn->name;
        sym->type = vdn->ntype;
        tab->add(sym);
    }

    sym::open(tab);

    this->body->validate();

    sym::close();
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

InnerStatementNode::~InnerStatementNode() {}

TreeComp StatementNode::type() {
    return TreeComp::STATEMENT;
}

StatementNode::~StatementNode() {
    delete this->inner;
}

void StatementNode::validate() {
    this->inner->validate();
}

TreeComp BlockStatementNode::type() {
    return TreeComp::BLOCK_STATEMENT;
}

BlockStatementNode::~BlockStatementNode() {
    for (auto stmt : this->statements) {
        delete stmt;
    }
}

void BlockStatementNode::validate() {
    sym::open(this->table);
    for (auto stmt : this->statements) {
        stmt->validate();
    }
    sym::close();
}

TreeComp ExprNode::type() {
    return TreeComp::EXPRESSION;
}

ExprNode::~ExprNode() {
    if (this->left != nullptr) delete this->left;
    if (this->right != nullptr) delete this->right;
}

void ExprNode::validate() {
    // TODO
}

TreeComp ControlFlowNode::type() {
    return TreeComp::FLOW_STATEMENT;
}

ControlFlowNode::~ControlFlowNode() {
    if (this->expression != nullptr) delete this->expression;
}

void ControlFlowNode::validate() {
    if (this->statement != "break")
        if (this->expression != nullptr)
            ZF_ERROR("found expression after %s statement", this->statement.c_str());
}

TreeComp VarDeclNode::type() {
    return TreeComp::VAR_DECL;
}

VarDeclNode::~VarDeclNode() {
    if (this->expr != nullptr) delete this->expr;
}

void VarDeclNode::validate() {
    this->expr->validate();
    auto* vsym = new sym::VariableSymbol();
    vsym->name = this->name;
    vsym->type = this->ntype;
    sym::tables[sym::tables.size() - 1]->add(vsym);
}

FuncCallNode::~FuncCallNode() {
    for (auto arg : this->args) {
        delete arg;
    }
}

void FuncCallNode::validate() {
    sym::FunctionSymbol* fn = static_cast<sym::FunctionSymbol*>(sym::lookup(this->name));
    if (fn == nullptr) {
        ZF_ERROR("could not find function %s", this->name.c_str());
    }
    if (fn->argtypes.size() != this->args.size()) {
        ZF_ERROR("incorrect number of arguments to function %s (expected %d, got %d)",
        this->name.c_str(), fn->argtypes.size(), this->args.size());
    }
    for (int i = 0; i < fn->argtypes.size(); i++) {
        if (this->args[i]->get_type() != fn->argtypes[i]) {
            ZF_ERROR("incorrect argument type to function %s (expected %s, got %s)",
            this->name.c_str(), fn->argtypes[i], this->args[i]->get_type());
        }
    }
}
