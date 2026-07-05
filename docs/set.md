A set data structure built on top of `List`. Ensures uniqueness of elements using a user-provided match function
## Structs

`Set` is a linked list with set-based operations available

```c
#define Set List
```

## Functions

### set_init

Initializes the set. The user is responsible for creating the initial `Set` structure and freeing memory with `set_destroy()`. `match` is a comparison function that returns `0` if two elements are equal (comparator convention, like `strcmp`). `destroy` is an optional deallocation function for the members of `Set`, pass `NULL` if the memory is stack-allocated.

```c
void set_init(Set *set, int (*match)(const void *a, const void *b),
              void (*destroy)(void *data));

/* Usage */
int int_match(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;  /* 0 = equal */
}

Set *set = malloc(sizeof(Set));
set_init(set, int_match, NULL);
```

### set_destroy

Destroys the nodes inside a `Set` and calls the deallocation function on the data if one was provided. Does not destroy the set itself, that is left up to the user.

```c
void set_destroy(Set *set);

/* Usage */
set_destroy(set);
free(set);
```

### set_insert

Inserts `data` into the set. If `data` is already a member (determined by the `match` function), the insertion is skipped and `1` is returned. Returns `0` on successful insertion, `-1` on error.

```c
int set_insert(Set *set, const void *data);

/* Usage */
int a = 1;
int b = 1;
set_insert(set, &a); // returns 0, inserted
set_insert(set, &b); // returns 1, duplicate skipped
```

### set_remove

Removes the element matching `*data` from the set. `data` is updated to point to the removed element's data so the user can manage its memory. Returns `0` on success, `-1` if the element is not found or if `data` is `NULL`.

```c
int set_remove(Set *set, void **data);

/* Usage */
int key = 1;
void *removed = &key;
set_remove(set, &removed);
```

### set_union

Computes the union of sets `a` and `b`, storing the result in `setu`. `setu` must be an uninitialized `Set` (it will be initialized inside the function). The result contains all elements from both sets with no duplicates. Returns `0` on success, `-1` on error.

```c
int set_union(Set *setu, const Set *a, const Set *b);

/* Usage */
Set *result = malloc(sizeof(Set));
set_union(result, set1, set2);
```

### set_intersection

Computes the intersection of sets `a` and `b`, storing the result in `seti`. `seti` must be an uninitialized `Set`. The result contains only elements present in both sets. Returns `0` on success, `-1` on error.

```c
int set_intersection(Set *seti, const Set *a, const Set *b);

/* Usage */
Set *result = malloc(sizeof(Set));
set_intersection(result, set1, set2);
```

### set_difference

Computes the difference of sets `a` and `b` (elements in `a` that are not in `b`), storing the result in `setd`. `setd` must be an uninitialized `Set`. Returns `0` on success, `-1` on error.

```c
int set_difference(Set *setd, const Set *a, const Set *b);

/* Usage */
Set *result = malloc(sizeof(Set));
set_difference(result, set1, set2);
```

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
#define set_size(s) ((s)->size)
```