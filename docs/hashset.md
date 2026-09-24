A set of unique `void *` values with hashed membership. Use [[set]] when you want union, intersection, and difference, or when the collection stays small.

`match` is the same equality callback as `Set` (`0` means equal). `hash` is required. There is no algebra API on this type.

## Usage

Initialize the set with equality and hash callbacks. Equal values must have equal hashes, and values must remain unchanged in ways that affect either callback while stored. The optional destructor runs on remaining members when the set is destroyed.

```c
int int_match(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b ? 0 : 1;
}

size_t int_hash(const void *p) {
    return (size_t)*(const int *)p;
}

LfHashSet set = {0};
if (lf_hashset_init(&set, 16, int_match, int_hash, NULL) == 0) {
    int value = 42;
    lf_hashset_insert(&set, &value);

    int key = 42;
    void *removed = &key;
    lf_hashset_remove(&set, &removed);
    lf_hashset_destroy(&set);
}
```

The example uses stack allocated data, so it passes `NULL` for the destructor. Removal returns the stored pointer without destroying it. The `LfHashSet` handle remains yours to free if you allocated it.

## Structs

### LfHashSet

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

Capacity is a power of two, at least 8. The table grows when live entries plus the new insert would exceed a 3/4 load. Deleted slots are reclaimed without increasing capacity. Duplicate inserts do not allocate or grow the table.

## Functions

### lf_hashset_init

`cap` is a hint and is rounded up. `match` and `hash` are required. Returns `0` on success, `-1` on error. Failed initialization leaves the handle unchanged; zero-initialize it if you need to call destroy after failure. Do not initialize a live set.

```c
int lf_hashset_init(LfHashSet *set, size_t cap,
                    int (*match)(const void *a, const void *b),
                    size_t (*hash)(const void *data),
                    void (*destroy)(void *data));

```

### lf_hashset_destroy

Calls `destroy` on remaining live members, frees the tables, and zeros the handle.

```c
void lf_hashset_destroy(LfHashSet *set);
```

### lf_hashset_insert

Returns `0` if inserted, `1` if already a member, `-1` on error. Duplicate and failed inserts leave the payload with the caller.

```c
int lf_hashset_insert(LfHashSet *set, const void *data);
```

### lf_hashset_remove

Removes the member matching `*data` and writes the stored payload back through `data`. Does not call `destroy`. Returns `0` on success, `-1` if missing or if the arguments are invalid.

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
