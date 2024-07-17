# memory

Custom allocators and memory functions

## Arena Allocator

A simple arena-style allocator

## Structs

### ArenaAllocator

Represents an arena allocator. `ArenaAllocator` holds its own buffer, but managing its size is left to the user. Like
most structs in `libflint`, it must be malloc'd first before being passed to `arena_init()`.

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

## Pool Allocator

A pool-based allocator for chunks of the same size. Useful for quickly allocating and using memory of the same size
without worrying about order; most operations are `O(1)`

## Structs

### PoolAllocator

Represents a pool allocator. `PoolAllocator` holds its own buffer, but managing its size is left to the user. Like
most structs in `libflint`, it must be malloc'd first before being passed to `pool_init()`.

```c
typedef struct {
	unsigned char *buf;
    size_t buf_sz;
    size_t chunk_size;

    List *free_list;
} PoolAllocator;
```

## Functions

### pool_init

Initializes the `PoolAllocator`. The struct must first be created by the user using `malloc()`, see the example below.

- `buf_sz`: Size of the underlying buffer in bytes
- `chunk_sz`: Size of each chunk in bytes
- `chunk_align`: The alignment of the chunks. The`LF_DEFAULT_ALIGNMENT` macro is available as a good default for basic types

```c
void pool_init(PoolAllocator *allocator, size_t buf_sz, size_t chunk_sz, size_t chunk_align);

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

Returns all chunks back to the pool. Any pointers received before this call are now invalid and must be reasigned with 
`pool_alloc` or set to `NULL`.

```c
void pool_free_all(PoolAllocator *allocator);
```

### pool_alloc

Allocate a chunk from the pool. Returns `NULL` on failure.

```c
void *pool_alloc(PoolAllocator *allocator);
```

### pool_destroy

Destroys the underlying buffer and `List` in `allocator`, then frees it. User is responsible for setting `allocator` to
`NULL` afterwards.

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

The default alignment for allocators. Use this as a simple default (defaults to 16 bytes on amd64 systems) when storing
basic types

```c
#ifndef DEFAULT_ALIGNMENT
#define LF_DEFAULT_ALIGNMENT (2*sizeof(void*))
#endif // DEFAULT_ALIGNMENT
```