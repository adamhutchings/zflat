#include "hashtable.h"

#include <stdint.h>

#define ZF_HASH_BUCKETS 1000000

struct zf_hashtable {
    void            * (buckets[ZF_HASH_BUCKETS]);
};

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

void zf_hashtable_set(
    struct zf_hashtable * ht, const char * key, void * value
) {
    uint32_t hash = zf_hash(key) % ZF_HASH_BUCKETS;
    ht->buckets[hash] = value;
}

void * zf_hashtable_get(struct zf_hashtable * ht, const char * key) {
   uint32_t hash = zf_hash(key) % ZF_HASH_BUCKETS;
   return ht->buckets[hash];
}
