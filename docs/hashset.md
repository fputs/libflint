A set of unique `void *` values with hashed membership. Use [[set]] when you want union, intersection, and difference, or when the collection stays small.

`match` is the same equality callback as `Set` (`0` means equal). `hash` is required. There is no algebra API on this type.

## Structs

```c
typedef struct {
    void **slots;
    unsigned char *state;
    size_t cap;
    size_t len;
    size_t tombs;
    int (*match)(const void *a, const void *b);
    size_t (*hash)(const void *data);
    void (*destroy)(void *data);
} LfHashSet;
```

Capacity is a power of two, at least 8. The table grows when live entries plus the new insert would exceed a 3/4 load.

## Functions

### lf_hashset_init

`cap` is a hint and is rounded up. `match` and `hash` are required. Returns `0` on success, `-1` on error.

```c
int lf_hashset_init(LfHashSet *set, size_t cap,
                    int (*match)(const void *a, const void *b),
                    size_t (*hash)(const void *data),
                    void (*destroy)(void *data));

/* Usage */
int int_match(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b ? 0 : 1;
}
size_t int_hash(const void *p) {
    return (size_t)*(const int *)p;
}

LfHashSet set;
lf_hashset_init(&set, 16, int_match, int_hash, NULL);
```

### lf_hashset_destroy

Calls `destroy` on remaining live members, frees the tables, and zeros the handle.

```c
void lf_hashset_destroy(LfHashSet *set);
```

### lf_hashset_insert

Returns `0` if inserted, `1` if already a member, `-1` on error.

```c
int lf_hashset_insert(LfHashSet *set, const void *data);
```

### lf_hashset_remove

Removes the member matching `*data` and writes the stored payload back through `data`. Does not call `destroy`. Returns `0` on success, `-1` if missing.

```c
int lf_hashset_remove(LfHashSet *set, void **data);
```

### lf_hashset_is_member

Returns `1` if `data` is present, `0` otherwise.

```c
int lf_hashset_is_member(const LfHashSet *set, const void *data);
```

## Macros

```c
#define lf_hashset_size(s) ((s)->len)
```
