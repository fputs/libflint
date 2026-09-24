A small collection of unique `void *` values. Membership is linear in the size of the set. Use the set functions to preserve uniqueness. For hashed membership, see [[hashset]].

## Usage

Initialize the set with an equality callback and an optional payload destructor. The equality callback returns `0` for equal values. This example passes `NULL` for the destructor because its payloads are stack allocated.

```c
int int_match(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b ? 0 : 1;
}

Set set;
if (set_init(&set, int_match, NULL) == 0) {
    int value = 1;
    int duplicate = 1;
    set_insert(&set, &value);
    set_insert(&set, &duplicate); /* Keeps value if the first insert succeeded. */
    set_destroy(&set);
}
```

`set_destroy` frees the nodes and calls the destructor on remaining payloads. It does not free the `Set` handle. Keep values unchanged in ways that affect equality while stored.

## Structs

### Set

```c
typedef struct {
    List impl;
} Set;
```

`match` is required at `set_init`. It returns `0` if two elements are equal (the same convention as `strcmp`). Compare with `==`, not subtraction, so signed overflow is not an issue.

## Functions

### set_init

Initializes the set. The user owns the `Set` handle. `match` is required; `destroy` is optional and is called on members only by `set_destroy`. Returns `0` on success, `-1` if `set` or `match` is NULL.

```c
int set_init(Set *set, int (*match)(const void *a, const void *b),
             void (*destroy)(void *data));

/* Usage */
int int_match(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b ? 0 : 1;
}

Set set;
set_init(&set, int_match, NULL);
```

### set_destroy

Destroys the nodes inside a `Set` and calls the deallocation function on the data if one was provided. Does not destroy the set itself, that is left up to the user.

```c
void set_destroy(Set *set);

/* Usage */
set_destroy(&set);
```

### set_insert

Inserts `data` into the set. If `data` is already a member (determined by the `match` function), the insertion is skipped and `1` is returned. Returns `0` on successful insertion, `-1` on error. Duplicate and failed inserts leave the payload with the caller.

```c
int set_insert(Set *set, const void *data);

/* Usage */
int a = 1;
int b = 1;
set_insert(&set, &a); /* returns 0, inserted */
set_insert(&set, &b); /* returns 1, duplicate skipped */
```

### set_remove

Removes the element matching `*data` from the set. `data` is updated to point to the removed element's data so the user can manage its memory. The destructor is not called. Returns `0` on success, `-1` if the element is not found or if `data` is `NULL`.

```c
int set_remove(Set *set, void **data);

/* Usage */
int key = 1;
void *removed = &key;
set_remove(&set, &removed);
```

### Algebra (borrowed)

`set_union`, `set_intersection`, and `set_difference` write into a destination you initialize. The destination must be empty, use the same `match` as `a` and `b`, and have `destroy == NULL`. Result members alias the payloads in `a` and `b`. Do not destroy `a` or `b` while the result is in use. `dest` cannot be `a` or `b`. Returns `0` on success, `-1` on error. Allocation failure leaves the destination empty and initialized.

```c
int set_union(Set *dest, const Set *a, const Set *b);
int set_intersection(Set *dest, const Set *a, const Set *b);
int set_difference(Set *dest, const Set *a, const Set *b);

/* Usage */
Set result;
set_init(&result, int_match, NULL);
set_union(&result, &set1, &set2);
/* use result, then: */
set_destroy(&result); /* nodes only; payloads still owned by set1/set2 */
```

Union contains every element from `a` or `b`. Intersection contains elements present in both. Difference is elements in `a` that are not in `b`.

### set_is_member

Returns `1` if `data` is a member of the set, `0` otherwise.

```c
int set_is_member(const Set *set, const void *data);
```

### set_is_subset

Returns `1` if `a` is a subset of `b` (every element in `a` is also in `b`), `0` otherwise.

```c
int set_is_subset(const Set *a, const Set *b);
```

### set_is_equal

Returns `1` if sets `a` and `b` contain the same elements, `0` otherwise.

```c
int set_is_equal(const Set *a, const Set *b);
```

## Macros

### set_size

Returns the number of elements in the set.

```c
#define set_size(s) ((s)->impl.size)
```
