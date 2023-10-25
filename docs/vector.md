# vector

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

Initialize the vector with a user-specified capacity. User is responsible for freeing elements of the vector with `vec_destroy()`. Returns a non-zero integer if there is an error

```c
int vec_init_with_capacity(Vector *vec, void (*destroy)(void *data), size_t cap);
```

### vec_destroy

Frees the underlying array inside a `Vector`. If `destroy` was provided when creating the vector, then it is called against each element in the array before the array is freed. Does not destroy the vector itself, that is left up to the user


```c
void vec_destroy(Vector *vec);

/* Usage */
vec_destroy(vec);
free(vec);
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

## Macros

### vec_at

Grabs the element at index `i` without safety checks for better performance.

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
