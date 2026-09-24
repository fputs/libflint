# lfhashmap

Include `lfhashmap.h`. `LfHashMap` is a caller-owned, open-addressed map of
pointer keys to pointer values. Initialize with an initial capacity (zero chooses
eight), equality callback (zero means equal), hash callback, and independent key
and value destruction callbacks. Equal keys must hash equally; keys must remain
stable while stored. NULL keys are allowed if both callbacks support them.

`lf_hashmap_insert` returns 0 on insertion, 1 on duplicate, and -1 on error.
Ownership passes only on success. `lf_hashmap_replace` replaces an existing pair,
returning 1 if absent. It destroys each displaced pointer except when identical
to the corresponding new pointer. Keys and values must have independent lifetimes
when using owning callbacks: do not share owned allocations across entries or
between key and value roles. Callbacks must not mutate the map.

`lf_hashmap_find(map, key, &value)` returns 0 for a hit, 1 for a miss, or -1 for
invalid arguments, so a stored NULL is distinct from a miss. `lf_hashmap_remove`
takes separate, required key and value outputs and transfers both pointers without
calling destructors. Outputs remain unchanged on failure and must not alias map
storage. `lf_hashmap_clear` invokes callbacks (including for NULL), retaining
capacity; destroy also frees storage and zeros the handle. NULL callbacks mean
borrowed pointers. NULL handles are accepted by clear and destroy.

Allocation arithmetic is checked. Failed insertion leaves contents and ownership
unchanged. Initialization failure leaves a destroyable handle; do not initialize
an already live map. Expected lookup/insertion/removal time is O(1); rehashing and
clear/destroy are O(capacity). No synchronization is provided.

## Structs

```c
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
```

## Functions

### lf_hashmap_init

Capacity is rounded up to a power of two, at least eight. Returns 0 on success,
-1 on invalid callbacks, size overflow, or allocation failure.

```c
int lf_hashmap_init(LfHashMap *map, size_t cap,
                    int (*match)(const void *, const void *),
                    size_t (*hash)(const void *),
                    void (*destroy_key)(void *), void (*destroy_value)(void *));

/* Usage: borrowed integer keys and values */
int int_match(const void *a, const void *b) {
    return *(const int *)a != *(const int *)b;
}
size_t int_hash(const void *p) { return (size_t)*(const int *)p; }

LfHashMap map;
if (lf_hashmap_init(&map, 16, int_match, int_hash, NULL, NULL) == 0) {
    int key = 42, value = 7;
    if (lf_hashmap_insert(&map, &key, &value) == 0) {
        void *found;
        if (lf_hashmap_find(&map, &key, &found) == 0) {
            /* found == &value */
        }
    }
    lf_hashmap_destroy(&map);
}
```

### lf_hashmap_insert

Returns 0 on insertion, 1 on duplicate, -1 on error. A duplicate keeps the
original pair and does not allocate. Failed insertion leaves ownership with the caller.

```c
int lf_hashmap_insert(LfHashMap *map, void *key, void *value);
```

### lf_hashmap_replace

Replaces only an existing pair. Returns 0 on replacement, 1 if absent, -1 on
invalid arguments. Destruction and pointer-identity rules are described above.

```c
int lf_hashmap_replace(LfHashMap *map, void *key, void *value);
```

### lf_hashmap_find

Returns 0 if found, 1 if absent, -1 on invalid arguments. The required output
borrows the stored value, which may be NULL.

```c
int lf_hashmap_find(const LfHashMap *map, const void *key, void **value);
```

### lf_hashmap_remove

Returns the same statuses as find. Both distinct output pointers are required;
success transfers the stored key and value without destruction.

```c
int lf_hashmap_remove(LfHashMap *map, const void *key,
                      void **stored_key, void **value);
```

### lf_hashmap_clear / lf_hashmap_destroy

Clear destroys all pairs and retains capacity. Destroy also releases the table
and zeros the handle.

```c
void lf_hashmap_clear(LfHashMap *map);
void lf_hashmap_destroy(LfHashMap *map);
```

## Macros

```c
#define lf_hashmap_size(m) ((m)->len)
```
