/**
 * A variably-sized linked list.
 */

#pragma once

struct zfll_node_t {

    void                    * data;
    struct zfll_node_t      * next;

};

struct zf_linked_list {
    struct zfll_node_t      * head;
};

/**
 * Initialize a new list.
 */
void zfll_init(struct zf_linked_list *);

/**
 * Add an element to the list.
 */
void zfll_add(struct zf_linked_list *, void *);

/**
 * Iterate over a list. The return value is the sum of all calls of the callback
 * function provided.
 */
int zfll_iterate(struct zf_linked_list *, int (*)(void *));

/**
 * Destroy a list.
 */
void zfll_destroy(struct zf_linked_list *);
