/**
 * A hash table which will be used for symbol tabless, and possibly other
 * things as well - although I haven't gotten there yet.
 */

#pragma once

/**
 * So that we can verify that a given lookup actually produced the correct key,
 * we store the key AND the value.
 */
struct zf_hash_entry {
    char            * key;
    void            * value;
};

#define ZF_HASH_BUCKETS         10000

struct zf_hashtable {
    struct zf_hash_entry     buckets[ZF_HASH_BUCKETS];
};

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

/**
 * Destroy a hash table.
 */
void zf_hashtable_destroy(struct zf_hashtable * ht);
