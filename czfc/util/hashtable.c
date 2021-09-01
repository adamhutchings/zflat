#include "hashtable.h"

#include <stdint.h>
#include <stdlib.h> /* free */
#include <string.h>

/**
 * How far down the hash table do we search for a new entry?
 */
#define ZF_HASH_SEARCH_DISTANCE 10

void zf_hashtable_init(struct zf_hashtable * table) {
    memset(table, 0, sizeof *table);
}

/**
 * Hashing algorithm - this is chosen more or less at random.
 */
static uint32_t zf_hash(const char * str) {

    uint32_t hash = 19329;

    while (*str) {
        hash = ((hash << 7) + hash) + *str;
        ++str;
    }

    return hash;

}

/**
 * Internal - find the bucket for getting or setting. Return the INDEX of the
 * bucket, or -1 if not found. Set create to 1 if we should create a new bucket,
 * or 0 if we should not.
 */
static int zf_hashtable_find_bucket(
    struct zf_hashtable * ht, const char * key, int create
) {

    uint32_t hash;
    uint32_t pos;
    
    hash = zf_hash(key) % ZF_HASH_BUCKETS;
    pos = hash; /* We iterate over this later. */

    /* We iterate starting at the hashed position, and check - if there's a
     * different key, go forward - if the key is the same, return this position,
     * if the key is empty, either go forward or create a new bucket, if the
     * flag is set.
     */
    for (; pos < hash + ZF_HASH_SEARCH_DISTANCE; ++pos) {
        if (ht->buckets[pos].key == NULL) {
            if (create) {
                ht->buckets[hash].key = strdup(key);
                return pos;
            }
        } else if (strcmp(key, ht->buckets[pos].key) == 0) {
            return pos;
        }
    }

    /* OK, if nothing else worked, we're done. */
    return -1;

}

int zf_hashtable_set(
    struct zf_hashtable * ht, const char * key, void * value
) {
    
    int bucket;

    bucket = zf_hashtable_find_bucket(ht, key, 1);

    if (bucket == -1) return 1;

    ht->buckets[bucket].value = value;
    return 0;

}

void * zf_hashtable_get(struct zf_hashtable * ht, const char * key) {
   
    int bucket;
    bucket = zf_hashtable_find_bucket(ht, key, 0);

    if (bucket == -1) return NULL;
    return ht->buckets[bucket].value;

}

void zf_hashtable_destroy(struct zf_hashtable * ht) {

    int i;
    char * key;

    for (i = 0; i < ZF_HASH_BUCKETS; ++i) {
        key = ht->buckets[i].key;
        if (key) {
            free(key);
        }
    }

}
