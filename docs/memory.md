Custom allocators and memory functions
# Arena Allocator

A simple arena-style allocator
## Structs

### ArenaAllocator

Represents an arena allocator. `ArenaAllocator` holds its own buffer. The handle may be stack- or heap-allocated.

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

Initializes the `ArenaAllocator`. `buf_sz` is the size of the underlying buffer in bytes. Returns `0` on success, `-1` on error (NULL allocator or malloc failure).

```c
int arena_init(ArenaAllocator *allocator, size_t buf_sz);

/* Usage */
ArenaAllocator a;
arena_init(&a, 1024);
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

A pool of same-sized chunks carved from one buffer. The free list is intrusive: a free chunk stores `void *next` in its first word. `pool_alloc` and `pool_free` do not call the system allocator. `lfmemory.h` does not include `lflinkedlist.h`.

The handle may be stack- or heap-allocated. `chunk_align` must be a non-zero power of two. Double-`pool_free` of the same live chunk is ignored so two later allocs cannot return the same address.

## Structs

### PoolAllocator

```c
typedef struct {
    unsigned char *buf;
    size_t buf_sz;
    size_t chunk_size;
    size_t aligned_start;
    size_t free_count;
    void *free_head;
} PoolAllocator;
```

## Functions

### pool_init

Initializes the `PoolAllocator`. Returns `0` on success, `-1` on error (NULL allocator, zero sizes, non-power-of-two or unsatisfiable `chunk_align`, malloc failure, or buffer too small for one aligned chunk).

- `buf_sz`: Size of the underlying buffer in bytes
- `chunk_sz`: Size of each chunk in bytes (rounded up to `chunk_align`; must fit a `void *`)
- `chunk_align`: Alignment of the chunks. `LF_DEFAULT_ALIGNMENT` is a good default for basic types

```c
int pool_init(PoolAllocator *allocator, size_t buf_sz, size_t chunk_sz, size_t chunk_align);

/* Usage */
PoolAllocator pool;
pool_init(&pool, 64, 16, LF_DEFAULT_ALIGNMENT);
```

### pool_free

Return a single chunk back to the pool. `ptr` is a pointer to the allocated chunk. Pointers outside the pool, misaligned pointers, NULL, and a chunk already on the free list are ignored.

```c
void pool_free(PoolAllocator *allocator, void *ptr);
```

### pool_free_all

Returns all chunks back to the pool. Any pointers received before this call are now invalid and must be reassigned with `pool_alloc` or set to `NULL`.

```c
void pool_free_all(PoolAllocator *allocator);
```

### pool_alloc

Allocate a chunk from the pool. Returns `NULL` on failure. The chunk is zeroed.

```c
void *pool_alloc(PoolAllocator *allocator);
```

### pool_destroy

Frees the underlying buffer and zeros the handle. Does not free the `allocator` struct itself.

```c
void pool_destroy(PoolAllocator *allocator);
```

## Macros

### pool_count_available

Returns the number of chunks left available in the pool.

```c
#define pool_count_available(x) ((x)->free_count)
```

### LF_DEFAULT_ALIGNMENT

The default alignment for allocators. Use this as a simple default (defaults to 16 bytes on amd64 systems) when storing basic types

```c
#ifndef LF_DEFAULT_ALIGNMENT
#define LF_DEFAULT_ALIGNMENT (2*sizeof(void*))
#endif // LF_DEFAULT_ALIGNMENT
```