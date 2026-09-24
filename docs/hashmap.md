A hash map of pointer keys to pointer values. Equal keys share one entry.

## Usage

Initialize a `LfHashMap` with equality and hash callbacks and optional key and value destructors. The equality callback returns `0` for equal keys. Equal keys must hash equally, and keys must remain stable while stored.

```c
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

The example uses stack allocated keys and values, so it passes `NULL` for both destructors. Removal returns both stored pointers without destroying them. Destroy frees the table and calls the destructors on remaining pairs; the `LfHashMap` handle is yours to free if you allocated it.

`NULL` keys are allowed if both callbacks support them. Stored `NULL` values are distinct from a missing key. Owned keys and values must have independent lifetimes: do not share owned allocations across entries or between key and value roles. Callbacks must not mutate the map. Concurrent mutation of the map is not supported.

Lookup and removal take expected O(1) time. Insertion takes amortized expected O(1) time; rehashing, clear, and destroy take O(capacity).

## Structs

### LfHashMapEntry

```c
typedef struct {
    void *key;
    void *value;
    unsigned char state;
} LfHashMapEntry;
```

### LfHashMap

```c
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

Capacity is rounded up to a power of two, at least eight. Returns `0` on success,
`-1` on a `NULL` handle, invalid callbacks, size overflow, or allocation failure. Failed initialization leaves a non-`NULL` handle destroyable. Do not initialize a live map.

```c
int lf_hashmap_init(LfHashMap *map, size_t cap,
                    int (*match)(const void *, const void *),
                    size_t (*hash)(const void *),
                    void (*destroy_key)(void *), void (*destroy_value)(void *));
```

### lf_hashmap_insert

Returns `0` on insertion, `1` on duplicate, `-1` on error. A duplicate keeps the
original pair and does not allocate. Failed insertion leaves contents and ownership unchanged.

```c
int lf_hashmap_insert(LfHashMap *map, void *key, void *value);
```

### lf_hashmap_replace

Replaces only an existing pair. Returns `0` on replacement, `1` if absent, `-1` on
invalid arguments. Calls each destructor on the displaced pointer unless it is identical to the corresponding new pointer. Ownership of the new pair passes only on success.

```c
int lf_hashmap_replace(LfHashMap *map, void *key, void *value);
```

### lf_hashmap_find

Returns `0` if found, `1` if absent, `-1` on invalid arguments. The required output
borrows the stored value, which may be `NULL`. The output is unchanged on failure and must not alias map storage.

```c
int lf_hashmap_find(const LfHashMap *map, const void *key, void **value);
```

### lf_hashmap_remove

Returns the same statuses as find. Both distinct output pointers are required;
success transfers the stored key and value without destruction. Outputs are unchanged on failure and must not alias map storage.

```c
int lf_hashmap_remove(LfHashMap *map, const void *key,
                      void **stored_key, void **value);
```

### lf_hashmap_clear / lf_hashmap_destroy

Clear destroys all pairs and retains capacity. Destroy also releases the table
and zeros the handle. Destructors are called even for `NULL` keys and values. Both functions accept a `NULL` handle.

```c
void lf_hashmap_clear(LfHashMap *map);
void lf_hashmap_destroy(LfHashMap *map);
```

## Macros

```c
#define lf_hashmap_size(m) ((m)->len)
```
