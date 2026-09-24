A runtime-sized compact array of bits. Include `lfbitset.h`. Extends [[bitops]]
from individual integers to dense integer sets. Handles are caller-owned; each
set owns its word array. All bits start cleared.

## Structs

```c
typedef struct {
    uint64_t *words;
    size_t nbits, nwords;
} LfBitSet;
```

Indexes are in `[0, nbits)`. Zero-sized sets are valid. Storage uses 64-bit words;
unused high bits of the final word stay zero. Treat fields as read-only.

## Functions

### lf_bitset_init

Returns 0 on success, -1 on invalid handle, allocation failure, or allocation
size overflow. Computes rounded word count without overflowing. Failure leaves
a destroyable empty handle. Do not initialize an already live set.

```c
int lf_bitset_init(LfBitSet *set, size_t nbits);

/* Usage */
LfBitSet visited;
if (lf_bitset_init(&visited, 100) == 0) {
    lf_bitset_set(&visited, 42);
    int present;
    if (lf_bitset_test(&visited, 42, &present) == 0) {
        /* present == 1 */
    }
    lf_bitset_destroy(&visited);
}
```

### lf_bitset_set / lf_bitset_clear / lf_bitset_toggle

Modify one bit. Return 0 on success, -1 for NULL handles or out-of-bounds indexes,
leaving the set unchanged on failure. These operations do not allocate.

```c
int lf_bitset_set(LfBitSet *set, size_t index);
int lf_bitset_clear(LfBitSet *set, size_t index);
int lf_bitset_toggle(LfBitSet *set, size_t index);
```

### lf_bitset_test

Write 0 or 1 to the required output. Returns 0 on success, -1 on invalid arguments
or index, leaving the output unchanged. The output must not alias set storage.

```c
int lf_bitset_test(const LfBitSet *set, size_t index, int *value);
```

### lf_bitset_count

Return the number of set bits; NULL returns zero. Unused tail bits are excluded.

```c
size_t lf_bitset_count(const LfBitSet *set);
```

### lf_bitset_union / lf_bitset_intersection

Combine into `dest` in place. Sizes must be equal; different sizes or NULL handles
return -1 without changing `dest`. Returns 0 on success. Self-combination is valid.
No allocation occurs, and unused tail bits are masked.

```c
int lf_bitset_union(LfBitSet *dest, const LfBitSet *src);
int lf_bitset_intersection(LfBitSet *dest, const LfBitSet *src);
```

### lf_bitset_destroy

Free storage and zero the handle. NULL and repeated destruction are accepted.

```c
void lf_bitset_destroy(LfBitSet *set);
```
