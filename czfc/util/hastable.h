/**
 * A hash table which will be used for symbol tabless, and possibly other
 * things as well - although I haven't gotten there yet.
 */

#pragma once

#include <stdint.h>

#define ZF_HASH_BUCKETS 1000000

struct zf_hashtable {
    void            * (buckets[ZF_HASH_BUCKETS]);
};

/**
 * Set an element in the hash table.
 */
void zf_hashtable_set(struct zf_hashtable * ht, const char * key, void * value);

/**
 * Get an element from the hash table.
 */
void * zf_hashtable_get(struct zf_hashtable * ht, const char * key);
