Arena and pool allocators. An arena allocates variable-sized blocks from one buffer; a pool reuses fixed-sized chunks.

## Usage

Include `lfmemory.h` and create the allocator handle. The handle belongs to you. Initialize it before allocating, and free its storage when you are finished.

```c
ArenaAllocator arena = {0};
if (arena_init(&arena, 1024) == 0) {
    int *value = arena_malloc(&arena, sizeof(*value));
    if (value != NULL) {
        *value = 42;
    }
    arena_free(&arena);
}
```

Arena allocations start zeroed and are aligned to `LF_DEFAULT_ALIGNMENT`. Do not call `free` on individual allocations. Clearing or freeing the arena invalidates all of them.

Use a pool when each allocation needs the same amount of space. Return individual chunks with `pool_free` so they can be reused.

```c
PoolAllocator pool = {0};
if (pool_init(&pool, 1024, 32, LF_DEFAULT_ALIGNMENT) == 0) {
    void *chunk = pool_alloc(&pool);
    pool_free(&pool, chunk);
    pool_destroy(&pool);
}
```

Do not initialize a live allocator again before releasing its storage. Zero-initialize handles if you need cleanup after a failed initialization.

## Structs

### ArenaAllocator

Stores the arena buffer and allocation offsets. The buffer is owned or borrowed, depending on the initializer. The handle may be stack- or heap-allocated.

```c
typedef struct {
    unsigned char* buf;
    size_t buf_sz;
    size_t offset_cur;
    size_t offset_prev;

    size_t save_stack[LF_ARENA_MAX_SAVE_POINTS];
    size_t save_count;
    int owns_buf;
} ArenaAllocator;
```

`owns_buf` is 1 when the arena malloc'd the slab (`arena_init`) and 0 when the caller provided it (`arena_init_buf`).

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

### arena_init

Initializes the `ArenaAllocator`. `buf_sz` is the size of the underlying buffer in bytes. Returns `0` on success, `-1` on error (NULL allocator or malloc failure).

```c
int arena_init(ArenaAllocator *allocator, size_t buf_sz);

/* Usage */
ArenaAllocator a;
arena_init(&a, 1024);
```

### arena_init_buf

Binds the arena to caller-owned storage (stack, static, or an existing mapping). The arena does not take ownership: `arena_free` detaches without freeing `storage`, and `arena_resize_buf` is rejected. `storage` must be non-NULL and `n` must be non-zero. Allocations are still aligned from the storage address. Returns `0` on success, `-1` on error.

```c
int arena_init_buf(ArenaAllocator *allocator, void *storage, size_t n);

/* Usage */
unsigned char buf[256];
ArenaAllocator a;
arena_init_buf(&a, buf, sizeof buf);
int *i = arena_malloc(&a, sizeof(int));
arena_free(&a); /* does not free buf */
```

### arena_free
If the arena owns its slab, frees it. Always resets the handle. Does not free the `allocator` struct itself, since the user is responsible for managing its lifetime (matching the behavior of `arena_init()`).
```c
void arena_free(ArenaAllocator *allocator);

/* Usage */
arena_free(&a); /* a is a stack-allocated handle */
```

### arena_clear

Resets the offset markers of the arena to `0` and clears all save points, but does not wipe the underlying buffer. All previous allocations become invalid. Requires a non-NULL initialized handle.

```c
void arena_clear(ArenaAllocator *allocator);
```


### arena_malloc

Allocates and zeros `size` bytes, aligned to `LF_DEFAULT_ALIGNMENT`. Returns `NULL` for a NULL handle, missing storage, or insufficient space.

```c
void *arena_malloc(ArenaAllocator* allocator, size_t size);
```

### arena_resize_buf

Reallocates the underlying buffer in the arena to `new_sz`. You can grow or shrink the arena using this function. Rejects zero size, shrinking below the current allocation offset, and arenas created with `arena_init_buf` (the slab is not owned). Any pointers allocated out of the arena are invalid after a successful resize. Returns `0` on success, `-1` on error (zero size, external buffer, shrink below offset, or realloc failure). Use `arena_free` to release the slab.

```c
int arena_resize_buf(ArenaAllocator *allocator, const size_t new_sz);
```

### arena_resize

Resizes an allocation, preserving the first `min(old_sz, new_sz)` bytes. Pass the allocation's actual size as `old_sz`; this function does not track block sizes. A NULL `mem` or zero `old_sz` requests a new allocation. Returns `NULL` if the arena doesn't have enough space or if `mem` doesn't belong to the arena. When growing, the new bytes are zeroed. See the example below for a simple use case.

```c
void *arena_resize(ArenaAllocator *allocator, void *mem, size_t old_sz, size_t new_sz);

/* Usage */
int *items = arena_malloc(&a, sizeof(int));
if (items != NULL) {
    items[0] = 1;
    int *grown = arena_resize(&a, items, sizeof(int), 2 * sizeof(int));
    if (grown != NULL) {
        items = grown;
        /* items[0] == 1, items[1] == 0 */
    }
}
```

### arena_save

Saves the current allocation offset onto an internal stack, allowing temporary allocations to be made and later discarded with `arena_restore()`. Requires a non-NULL initialized handle. Supports nesting up to `LF_ARENA_MAX_SAVE_POINTS` (default 32). Returns `0` on success, `-1` if the save stack is full.

```c
int arena_save(ArenaAllocator *allocator);
```

### arena_restore

Restores the arena offset to the most recent save point, effectively freeing all allocations made since the last `arena_save()` call. Does nothing if no save points exist. Allocations made before the save point are unaffected, provided they were not resized during the saved scope. Requires a non-NULL initialized handle.

```c
void arena_restore(ArenaAllocator *allocator);

/* Usage */
if (arena_save(&a) == 0) {
    int *tmp = arena_malloc(&a, sizeof(int));
    if (tmp != NULL) {
        *tmp = 999;
    }
    arena_restore(&a);  /* tmp is now invalid */
}
```

### LF_ARENA_MAX_SAVE_POINTS

Maximum number of nested save points for `arena_save()`/`arena_restore()`. Can be overridden when building the library and its callers; use the same value in both because it changes the handle layout. Defaults to 32.

```c
#ifndef LF_ARENA_MAX_SAVE_POINTS
#define LF_ARENA_MAX_SAVE_POINTS 32
#endif
```


### pool_init

Creates a pool of equally sized chunks. Free chunks store a next pointer in their first word; allocation and return do not call the system allocator.

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

### pool_count_available

Returns the number of chunks left available in the pool.

```c
#define pool_count_available(x) ((x)->free_count)
```

### LF_DEFAULT_ALIGNMENT

The alignment used by arena allocations, and a suggested alignment for pool chunks. Defaults to twice the pointer size (16 bytes on amd64). It must be a nonzero power of two. To change arena alignment, define it when building the library.

```c
#ifndef LF_DEFAULT_ALIGNMENT
#define LF_DEFAULT_ALIGNMENT (2*sizeof(void*))
#endif // LF_DEFAULT_ALIGNMENT
```
