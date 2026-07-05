Custom allocators and memory functions
# Arena Allocator

A simple arena-style allocator
## Structs

### ArenaAllocator

Represents an arena allocator. `ArenaAllocator` holds its own buffer, but managing its size is left to the user. Like most structs in `libflint`, it must be malloc'd first before being passed to `arena_init()`.

```c
typedef struct {
    unsigned char* buf;
    size_t buf_sz;
    size_t offset_cur;
    size_t offset_prev;

    size_t save_stack[LF_ARENA_MAX_SAVE_POINTS];
    size_t save_count;
} ArenaAllocator;
```

## Functions

### arena_init

Initializes the `ArenaAllocator`. The struct must first be created by the user using `malloc()`, see the example below. `buf_sz` is the size of the underlying buffer in bytes. Returns `0` on success, `-1` on error (NULL allocator or malloc failure).

```c
int arena_init(ArenaAllocator *allocator, size_t buf_sz);

/* Usage */
ArenaAllocator *a = malloc(sizeof(ArenaAllocator));
arena_init(a, 1024);
```

### arena_free
Frees the underlying buffer and resets all fields in `allocator` to zero. Does not free the `allocator` struct itself, since the user is responsible for managing its lifetime (matching the behavior of `arena_init()`).
```c
void arena_free(ArenaAllocator *allocator);

/* Usage */
arena_free(a);
free(a); // if heap-allocated
a = NULL;
```

### arena_clear

Resets the offset markers of the arena to `0` and clears all save points, but does not wipe the underlying buffer.

```c
void arena_clear(ArenaAllocator *allocator);
```


### *arena_malloc

Request memory of `size` bytes in length from the arena. Returns `NULL` if the assignment failed.

```c
void *arena_malloc(ArenaAllocator* allocator, size_t size);
```

### arena_resize_buf

Reallocates the underlying buffer in the arena to `new_sz`. You can grow or shrink the arena using this function. Rejects shrinking below the current allocation offset. Any pointers allocated out of the arena are invalid after using this function. Returns `0` on success, `-1` on error (shrink below offset or realloc failure).

```c
int arena_resize_buf(ArenaAllocator *allocator, const size_t new_sz);
```

### *arena_resize

Resize an allocated pointer from the arena. Returns `NULL` if the arena doesn't have enough space or if `mem` doesn't belong to the arena. When growing, the new bytes are zeroed. See the example below for a simple use case.

```c
void *arena_resize(ArenaAllocator *allocator, void *mem, size_t old_sz, size_t new_sz);

/* Usage */
int *i = arena_malloc(a, sizeof(int));
*i = 1;
long *l = arena_resize(a, i, sizeof(int), sizeof(long));
assert(*l == 1);
```

### arena_save

Saves the current allocation offset onto an internal stack, allowing temporary allocations to be made and later discarded with `arena_restore()`. Supports nesting up to `LF_ARENA_MAX_SAVE_POINTS` (default 32). Returns `0` on success, `-1` if the save stack is full.

```c
int arena_save(ArenaAllocator *allocator);
```

### arena_restore

Restores the arena offset to the most recent save point, effectively freeing all allocations made since the last `arena_save()` call. Does nothing if no save points exist. Permanent allocations made before the save point are unaffected.

```c
void arena_restore(ArenaAllocator *allocator);

/* Usage */
int *permanent = arena_malloc(a, sizeof(int));
*permanent = 42;

arena_save(a);
int *tmp = arena_malloc(a, sizeof(int));
*tmp = 999;  // temporary work
arena_restore(a);  // tmp is now invalid, permanent is intact

int *next = arena_malloc(a, sizeof(int));  // reuses tmp's space
```

## Macros

### LF_ARENA_MAX_SAVE_POINTS

Maximum number of nested save points for `arena_save()`/`arena_restore()`. Can be overridden by defining before including the header. Defaults to 32.

```c
#ifndef LF_ARENA_MAX_SAVE_POINTS
#define LF_ARENA_MAX_SAVE_POINTS 32
#endif
```

# Pool Allocator

A pool-based allocator for chunks of the same size. Useful for quickly allocating and using memory of the same size without worrying about order; most operations are `O(1)`

## Structs

### PoolAllocator

Represents a pool allocator. `PoolAllocator` holds its own buffer, but managing its size is left to the user. Like most structs in `libflint`, it must be malloc'd first before being passed to `pool_init()`.

```c
typedef struct {
    unsigned char *buf;
    size_t buf_sz;
    size_t chunk_size;
    size_t aligned_start;

    List *free_list;
} PoolAllocator;
```

## Functions

### pool_init

Initializes the `PoolAllocator`. The struct must first be created by the user using `malloc()`, see the example below. Returns `0` on success, `-1` on error (NULL allocator, malloc failure, invalid chunk size, or buffer too small to fit alignment and at least one chunk).

- `buf_sz`: Size of the underlying buffer in bytes
- `chunk_sz`: Size of each chunk in bytes
- `chunk_align`: The alignment of the chunks. The`LF_DEFAULT_ALIGNMENT` macro is available as a good default for basic types

```c
int pool_init(PoolAllocator *allocator, size_t buf_sz, size_t chunk_sz, size_t chunk_align);

/* Usage */
PoolAllocator *pool = malloc(sizeof(PoolAllocator));
pool_init(pool, 64, 16, LF_DEFAULT_ALIGNMENT);
```

### pool_free

Return a single chunk back to the pool. `ptr` is a pointer to the allocated chunk.

```c
void pool_free(PoolAllocator *allocator, void *ptr);
```

### pool_free_all

Returns all chunks back to the pool. Any pointers received before this call are now invalid and must be reasigned with `pool_alloc` or set to `NULL`.

```c
void pool_free_all(PoolAllocator *allocator);
```

### pool_alloc

Allocate a chunk from the pool. Returns `NULL` on failure.

```c
void *pool_alloc(PoolAllocator *allocator);
```

### pool_destroy

Destroys the underlying buffer and `List` in `allocator`. Does not free the `allocator` struct itself — the user is responsible for freeing it (matching the convention of other libflint modules).

```c
void pool_destroy(PoolAllocator *allocator);
```

## Macros

### pool_count_available

Returns the amount of chunks left available in the pool

```c
#define pool_count_available(x) (x)->free_list->size
```

### LF_DEFAULT_ALIGNMENT

The default alignment for allocators. Use this as a simple default (defaults to 16 bytes on amd64 systems) when storing basic types

```c
#ifndef LF_DEFAULT_ALIGNMENT
#define LF_DEFAULT_ALIGNMENT (2*sizeof(void*))
#endif // LF_DEFAULT_ALIGNMENT
```