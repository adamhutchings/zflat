/**
 * A hash table which will be used for symbol tabless, and possibly other
 * things as well - although I haven't gotten there yet.
 */

#pragma once

struct zf_hashtable;

/**
 * Initialize a hash table.
 */
void zf_hashtable_init(struct zf_hashtable * table);

/**
 * Set an element in the hash table. Return 1 if failed.
 */
int zf_hashtable_set(struct zf_hashtable * ht, const char * key, void * value);

/**
 * Get an element from the hash table.
 */
void * zf_hashtable_get(struct zf_hashtable * ht, const char * key);
