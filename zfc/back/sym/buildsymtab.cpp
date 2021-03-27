#include <ast/ast.hpp>
#include <back/sym/symtab.hpp>

void BlockStatementNode::build_symtab() {

    this->table = new sym::SymbolTable();

    VarDeclNode* vd;

    for (StatementNode* st : this->statements) {
        if ( (vd = dynamic_cast<VarDeclNode*> (st)) != nullptr ) {
            // We have a variable declaration
            sym::Symbol* sym = new sym::Symbol;
            sym->name = vd->name;
            sym->type = vd->ntype;
            this->table->add(sym);
        }
    }

}
