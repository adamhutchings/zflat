#include "hashtable.h"

#include <stdint.h>
#include <string.h>

#define ZF_HASH_BUCKETS         10000

/**
 * How far down the hash table do we search for a new entry?
 */
#define ZF_HASH_SEARCH_DISTANCE 10

/**
 * So that we can verify that a given lookup actually produced the correct key,
 * we store the key AND the value.
 */
struct zf_hash_entry {
    char            * key;
    void            * value;
};

struct zf_hashtable {
    struct zf_hash_entry     buckets[ZF_HASH_BUCKETS];
};

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

int zf_hashtable_set(
    struct zf_hashtable * ht, const char * key, void * value
) {
    uint32_t hash;
    uint32_t pos;
    
    hash = zf_hash(key) % ZF_HASH_BUCKETS;

    /* Three cases. Either we create a new value, we replace an old value, or
     * this slot is already filled so we look for the next free one. */
    if (ht->buckets[hash].key == NULL) {
        ht->buckets[hash] = (struct zf_hash_entry) {
            .key   = strdup(key),
            .value = value
        };
    } else if (strcmp(key, ht->buckets[hash].key) == 0) {
        ht->buckets[hash].value = value;
    } else {

        for (pos = hash; pos < hash + ZF_HASH_SEARCH_DISTANCE; ++pos) {
            if (ht->buckets[pos].key == NULL) {
                ht->buckets[pos] = (struct zf_hash_entry) {
                    .key   = strdup(key),
                    .value = value
                };
                return 0;
            } else if (strcmp(key, ht->buckets[pos].key) == 0) {
                ht->buckets[pos].value = value;
                return 0;
            }
        }

        /* OK, if that for loop didn't return anything, this is not good. */
        return 1;

    }

}

void * zf_hashtable_get(struct zf_hashtable * ht, const char * key) {
   uint32_t hash = zf_hash(key) % ZF_HASH_BUCKETS;
   return ht->buckets[hash];
}
