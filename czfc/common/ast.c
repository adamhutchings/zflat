/**
 * Recursive application definitions from ast.c
 */

#include "ast.h"

static int (*zfa_op)(struct zfa_node * node);

void zfa_node_setop(int (*op)(struct zfa_node *)) {
    zfa_op = op;
}

int zfa_node_apply(struct zfa_node * node) {

    int ret;
    int (*opcast)(void*) = (int (*)(void*))zfa_op;
    ret = 0;

    switch (node->type) {
    case ZFA_NODE_NONE:
    case ZFA_NODE_IDENT:
    case ZFA_NODE_VALUE:
        break;
    case ZFA_NODE_PROG:
        ret += zfll_iterate(&node->as.blockstmt.stmts, opcast);
        break;
    case ZFA_NODE_EXPR:
        ret += zfa_node_apply(node->as.expr.left);
        ret += zfa_node_apply(node->as.expr.right);
        break;
    case ZFA_NODE_DECL:
        ret += zfa_node_apply(node->as.decl.expr);
        break;
    case ZFA_NODE_FUNCCALL:
        ret += zfa_node_apply(node->as.funccall.ident);
        ret += zfll_iterate(&node->as.funccall.params, opcast);
        break;
    case ZFA_NODE_BLOCK_STMT:
        ret += zfll_iterate(&node->as.blockstmt.stmts, opcast);
        break;
    case ZFA_NODE_FUNCTION:
        ret += zfa_node_apply(node->as.function.decl);
        ret += zfll_iterate(&node->as.function.params, opcast);
        ret += zfa_node_apply(node->as.function.body);
        break;
    }

    ret += zfa_op(node);
    return ret;

}
