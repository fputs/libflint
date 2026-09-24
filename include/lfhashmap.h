#ifndef LIBFLINT_H_HASHMAP
#define LIBFLINT_H_HASHMAP

#include <stddef.h>

typedef struct {
    void *key;
    void *value;
    unsigned char state;
} LfHashMapEntry;

typedef struct {
    LfHashMapEntry *entries;
    size_t cap, len, tombs;
    int (*match)(const void *, const void *);
    size_t (*hash)(const void *);
    void (*destroy_key)(void *);
    void (*destroy_value)(void *);
} LfHashMap;

#define lf_hashmap_size(m) ((m)->len)

/* match returns zero for equal keys, which must have equal hashes. */
int lf_hashmap_init(LfHashMap *map, size_t cap,
                    int (*match)(const void *, const void *),
                    size_t (*hash)(const void *),
                    void (*destroy_key)(void *), void (*destroy_value)(void *));
/* 0: inserted, 1: duplicate, -1: error; ownership passes only on success. */
int lf_hashmap_insert(LfHashMap *map, void *key, void *value);
/* Replace an existing pair only: 0 success, 1 absent, -1 invalid arguments.
 * Destroys old key/value unless identical to their respective replacements. */
int lf_hashmap_replace(LfHashMap *map, void *key, void *value);
/* 0 found, 1 absent, -1 invalid arguments. Outputs unchanged on failure. */
int lf_hashmap_find(const LfHashMap *map, const void *key, void **value);
/* Both outputs required; removal transfers both pointers to the caller. */
int lf_hashmap_remove(LfHashMap *map, const void *key,
                      void **stored_key, void **value);
void lf_hashmap_clear(LfHashMap *map);
void lf_hashmap_destroy(LfHashMap *map);

#endif
