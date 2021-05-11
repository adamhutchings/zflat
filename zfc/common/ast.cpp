#include "ast.hpp"

namespace {
void pdel(ASTNode* node) { if (dynamic_cast<ProgramNode*>(node) == nullptr) delete node; }
}

ProgramNode::~ProgramNode() {
    this->apply(pdel);
}

void ProgramNode::apply(void(*fn)(ASTNode*)) {
    for (auto sub : this->components) {
        sub->apply(fn);
    }
    fn(this);
}

void FunctionNode::apply(void(*fn)(ASTNode*)) {
    // for (auto decl : this->args) {
    //     fn(decl);
    // }
    this->body->apply(fn);
    fn(this);
}

void StatementNode::apply(void(*fn)(ASTNode*)) {
    if (this->inner != nullptr)
        this->inner->apply(fn);
    fn(this);
}

void BlockStatementNode::apply(void(*fn)(ASTNode*)) {
    for (auto stmt : this->statements) {
        stmt->apply(fn);
    }
    fn(this);
}

void IfNode::apply(void(*fn)(ASTNode*)) {
    this->expr->apply(fn);
    this->stmt->apply(fn);
    if (this->else_block != nullptr) this->else_block->apply(fn);
    fn(this);
}

void LoopNode::apply(void(*fn)(ASTNode*)) {
    this->expr->apply(fn);
    if (this->pred != nullptr) this->pred->apply(fn);
    this->stmt->apply(fn);
    fn(this);
}

void SwitchNode::apply(void(*fn)(ASTNode*)) {
    this->expr->apply(fn);
    for (auto case_ : this->cases) {
        case_->apply(fn);
    }
    fn(this);
}

void CaseNode::apply(void(*fn)(ASTNode*)) {
    for (auto st : this->statements) {
        st->apply(fn);
    }
}

void ControlFlowNode::apply(void(*fn)(ASTNode*)) {
    if (this->expression != nullptr) this->expression->apply(fn);
    fn(this);
}

void VarDeclNode::apply(void(*fn)(ASTNode*)) {
    if (this->expr != nullptr) this->expr->apply(fn);
    fn(this);
}

void EnumDeclNode::apply(void(*fn)(ASTNode*)) {
    fn(this);
}

void UseNode::apply(void(*fn)(ASTNode*)) {
    
}

void warn_unused_value(ProgramSub* pn) {

    auto sn = dynamic_cast<StatementNode*>(pn);
    if (sn != nullptr) {
        auto exp = dynamic_cast<ExprNode*>(sn->inner);
        if (exp != nullptr) {
            auto bexp = dynamic_cast<BinaryExprNode*>(exp->inner);
            if (bexp != nullptr) {
                if (bexp->op != op::Operator::INVALID && !op::is_assign(bexp->op)) {
                    fprintf(stderr, "zfc: warning: line %d: expression value unused\n", exp->line);
                }
            }
        }
    }

    auto fn = dynamic_cast<FunctionNode*>(pn);
    if (fn != nullptr && !(fn->symbol->extc)) {
        for (auto st : fn->body->statements) {
            warn_unused_value(st);
        }
    }

}
