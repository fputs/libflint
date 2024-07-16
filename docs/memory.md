# memory

Custom allocators and memory functions

# Arena Allocator

A simple arena-style allocator

## Structs

### ArenaAllocator

Represents an arena allocator. `ArenaAllocator` holds its own buffer, but managing its size is left to the user. Like
most structs in `libflint`, it must be malloced first before being passed to `arena_init()`.

```c
typedef struct {
    unsigned char* buf;
    size_t buf_sz;
    size_t offset_cur;
    size_t offset_prev;
} ArenaAllocator;
```

## Functions

### arena_init

Initializes the `ArenaAllocator`. The struct must first be created by the user using `malloc()`, see the example below.
`buf_sz` is the size of the underlying buffer in bytes.

```c
void arena_init(ArenaAllocator *allocator, size_t buf_sz);

/* Usage */
ArenaAllocator *a = malloc(sizeof(ArenaAllocator));
arena_init(a, 1024);
```

### arena_free
Frees `allocator` and its underlying buffer. Users should set `allocator` to `NULL` after calling `arena_free()`.
```c
void arena_free(ArenaAllocator *allocator);

/* Usage */
arena_free(allocator);
allocator = NULL;
```

### arena_clear

Resets the offset markers of the arena to `0`, but does not wipe the underlying buffer. Technically, any assigned pointers
will still work and

```c
void arena_clear(ArenaAllocator *allocator);
```


### *arena_malloc

Request memory of `size` bytes in length from the arena. Returns `NULL` if the assignment failed.

```c
void *arena_malloc(ArenaAllocator* allocator, size_t size);
```

### arena_resize_buf

Reallocates the underlying buffer in the arena to `new_sz`. You can grow or shrink the arena using this function. Any
pointers allocated out of the arena are invalid after using this function.

```c
void arena_resize_buf(ArenaAllocator *allocator, size_t new_sz);
```

### *arena_resize

Resize an allocated pointer from the arena. See the example below for a simple use case

```c
void *arena_resize(ArenaAllocator *allocator, void *mem, size_t old_sz, size_t new_sz);

/* Usage */
int *i = arena_malloc(a, sizeof(int));
*i = 1;
long *l = arena_resize(a, i1, sizeof(int), sizeof(long));
assert(*l == 1);
```

## Macros

### arena_sz

Convenience macro for getting the size of the arena's buffer

```c
#define arena_sz(a) (a)->buf_sz
```