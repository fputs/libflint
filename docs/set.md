A small uniqueness-constrained collection over a `List`. Membership is O(n). The public type is a wrapper, not a `#define`, so list operations do not compile against a `Set` and cannot break uniqueness. For amortized O(1) membership, see [[hashset]].

## Structs

```c
typedef struct {
    List impl;
} Set;
```

`match` is required at `set_init`. It returns `0` if two elements are equal (comparator convention, like `strcmp`). Equality is `==`, not `a - b` (signed overflow).

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

Inserts `data` into the set. If `data` is already a member (determined by the `match` function), the insertion is skipped and `1` is returned. Returns `0` on successful insertion, `-1` on error.

```c
int set_insert(Set *set, const void *data);

/* Usage */
int a = 1;
int b = 1;
set_insert(&set, &a); /* returns 0, inserted */
set_insert(&set, &b); /* returns 1, duplicate skipped */
```

### set_remove

Removes the element matching `*data` from the set. `data` is updated to point to the removed element's data so the user can manage its memory. Returns `0` on success, `-1` if the element is not found or if `data` is `NULL`.

```c
int set_remove(Set *set, void **data);

/* Usage */
int key = 1;
void *removed = &key;
set_remove(&set, &removed);
```

### Algebra (borrowed)

`set_union`, `set_intersection`, and `set_difference` write a borrowed view into a caller-initialized destination. The destination must be empty, share `a` and `b`'s `match`, and have `destroy == NULL`. Result nodes alias the input payloads — do not destroy `a` or `b` while the view is live. `dest == a` and `dest == b` are rejected (that path used to leak `a`'s nodes). The functions never call `set_init` on a destination the caller already owns. Returns `0` on success, `-1` on error.

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
