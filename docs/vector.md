Simple type-agnostic dynamic array
## Structs

### Vector

Vector struct

```c
typedef struct Vector {
size_t capacity;
size_t length;
void **elements;
void (*destroy)(void *data);
} Vector;
```

Members:

- `capacity`: The size of `elements` in memory
- `length`: The number of real elements stored in the backing `elements` array
- `elements`: The dynamic array of `void *` that holds the vector's members
- `destroy`: Optional deallocation function for data inside a node. Typical usage is `NULL` for stack allocated data and `free()` for data created with `malloc()`

## Functions

### vec_init

Initialize the vector with a default capacity of 2. User is responsible for freeing elements of the vector with `vec_destroy()`. Returns a non-zero integer if there is an error

```c
int vec_init(Vector *vec, void (*destroy)(void *data));
```

### vec_init_with_capacity

Initialize the vector with a user-specified capacity. `capacity == 0` is allowed: `elements` is `NULL` and the first insert grows from the default capacity. Failed init leaves a destroyable empty vector (`capacity == 0`, `elements == NULL`). Returns a non-zero integer if there is an error.

```c
int vec_init_with_capacity(Vector *vec, void (*destroy)(void *data), size_t cap);
```

### vec_destroy

Frees the underlying array inside a `Vector`. If `destroy` was provided when creating the vector, then it is called against each element in the array before the array is freed. Does not destroy the vector itself, that is left up to the user. Zeros the handle so a second `vec_destroy` is a no-op.

```c
void vec_destroy(Vector *vec);

/* Usage */
vec_destroy(vec);
free(vec);
```

### vec_clear

Destroys all elements in the vector (calling `destroy` on each if provided) and sets length to 0. Capacity and the backing array are kept. Returns `0` on success, `-1` on error.

```c
int vec_clear(Vector *vec);
```

### vec_grow_to

Grows the capacity of the vector to `new_cap`. Returns `-1` if `new_cap` is less than the current capacity. Returns `0` without reallocating when `new_cap` equals the current capacity.

```c
int vec_grow_to(Vector *vec, const size_t new_cap);
```

### vec_reserve

Ensure the vector can hold at least `n` elements. No-op (returns `0`) when `n` is already within capacity.

```c
int vec_reserve(Vector *vec, size_t n);
```

### vec_insert

Insert `data` into the vector at a specified index. Any elements at that array and beyond will be moved up to make room. Returns a non-zero integer if there is an error

```c
int vec_insert(Vector *vec, void *data, size_t index);

/* Usage */

// vec: 1 2 3 4
int i = 99;
vec_insert(vec, &i, 2);
// vec: 1 2 99 3 4
```

### vec_push

Insert `data` at the end of the vector. Returns a non-zero integer if there is an error

```c
int vec_push(Vector *vec, void *data);

/* Usage */

// vec: 1 2 3
int *i = malloc(sizeof(int));
*i = 4;
vec_push(vec, i);
// vec: 1 2 3 4
```

### vec_safe_at

Gets the stored data at a specified index. Uses safety checks to make sure `index` is not out of bounds. Returns `NULL` if there is an error

```c
void *vec_safe_at(Vector *vec, size_t index);
```

### vec_remove

Removes an element from the array and returns the pointer, then moves elements to shrink the array accordingly. Returns `NULL` if the index is not valid

```c
void *vec_remove(Vector *vec, size_t index);

/* Usage */
// vec: 1 2 3 4
int *t = NULL;
t = (int*)vec_remove(vec, 2);
assert(*t == 3);
// vec: 1 2 4
```

### vec_shrink

Shrinks the capacity of the vector down to the current length. Returns a non-zero integer if there is an error

```c
int vec_shrink(Vector *vec);
```

### vec_min

Finds the smallest value in the vector and returns a void pointer to the underlying data. Returns `NULL` if the vector is empty. Requires a comparison function to compare the data in the vector. This function must return `1` if `a > b`, `-1` if `a < b`, or `0` if `a == b`. See the supplied comparison functions below for reference

```c
const void *vec_min(const Vector *vec, int(*cmp)(const void *a, const void *b));
```

### vec_max

Finds the largest value in the vector and returns a void pointer to the underlying data. Returns `NULL` if the vector is empty. Requires a comparison function to compare the data in the vector. This function must return `1` if `a > b`, `-1` if `a < b`, or `0` if `a == b`. See the supplied comparison functions below for reference

```c
const void *vec_max(const Vector *vec, int(*cmp)(const void *a, const void *b));
```

### vec_pop

Removes and returns the last element of the vector. Returns `NULL` if the vector is empty.

```c
void *vec_pop(Vector *vec);

/* Usage */
// vec: 1 2 3
int *t = (int *)vec_pop(vec);
assert(*t == 3);
// vec: 1 2
```

### vec_reverse

Reverses the order of elements in the vector in-place. O(n) time, O(1) space.

```c
void vec_reverse(Vector *vec);
```

### vec_sort

Sorts the vector in-place using `qsort_r` (thread-safe). Requires a comparator function that returns a negative value if `a < b`, `0` if `a == b`, or a positive value if `a > b`. See the supplied comparison functions below for reference.

```c
void vec_sort(Vector *vec, int (*cmp)(const void *a, const void *b));

/* Usage */
vec_sort(vec, vec_cmp_int);
```

### vec_bsearch

Binary search on a sorted vector. Returns a pointer to the matching element's data, or `NULL` if not found. The vector must be sorted by the same comparator before calling this function.

```c
void *vec_bsearch(Vector *vec, const void *key, int (*cmp)(const void *a, const void *b));

/* Usage */
vec_sort(vec, vec_cmp_int);
int key = 3;
int *found = (int *)vec_bsearch(vec, &key, vec_cmp_int);
```

## Comparison Functions

Comparison functions to compare data in a vector. These functions must return `1` if `a > b`, `-1` if `a < b`, or `0` if `a == b`.

```c
int vec_cmp_int(const void *a, const void *b);
int vec_cmp_char(const void *a, const void *b);
```

## Macros

### vec_at

Grabs the element at index `i` without safety checks for better performance. Use with caution

```c
#define vec_at(v, i) (v)->elements[(i)]
```

### vec_len

Returns the length of the vector

```c
#define vec_len(v) (v)->length
```

### vec_cap

Returns the capacity of the vector

```c
#define vec_cap(v) (v)->capacity
```