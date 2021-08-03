#include "linkedlist.h"

#include <stdlib.h>

void zfll_init(struct zf_linked_list * list) {

    list->head = NULL;

}

void zfll_add(struct zf_linked_list * list, void * elem) {

    struct zfll_node_t * node;
    
    node = malloc(sizeof *node);
    node->data = elem;
    node->next = list->head;
    list->head = node;

}

int zfll_iterate(struct zf_linked_list * list, int (*callback)(void *)) {

    struct zfll_node_t * node;
    int                  sum;

    for (node = list->head, sum = 0; node != NULL; node = node->next) {
        sum += callback(node->data);
    }

    return sum;

}
