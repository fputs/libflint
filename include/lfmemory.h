#ifndef LIBFLINT_H_MEMORY
#define LIBFLINT_H_MEMORY

#include <stddef.h>

#include "lflinkedlist.h"

#ifndef LF_DEFAULT_ALIGNMENT
#define LF_DEFAULT_ALIGNMENT (2*sizeof(void*))
#endif // LF_DEFAULT_ALIGNMENT

typedef struct {
    unsigned char* buf;
    size_t buf_sz;
    size_t offset_cur;
    size_t offset_prev;
} ArenaAllocator;

void arena_init(ArenaAllocator *allocator, size_t buf_sz);
void arena_free(ArenaAllocator *allocator);
void *arena_malloc(ArenaAllocator* allocator, size_t size);
void arena_resize_buf(ArenaAllocator *allocator, size_t new_sz);
void *arena_resize(ArenaAllocator *allocator, void *mem, size_t old_sz, size_t new_sz);
void arena_clear(ArenaAllocator *allocator);

typedef struct {
	unsigned char *buf;
    size_t buf_sz;
    size_t chunk_size;

    List *free_list;
} PoolAllocator;

void pool_init(PoolAllocator *allocator, size_t buf_sz, size_t chunk_sz, size_t chunk_align);
void pool_free(PoolAllocator *allocator, void *ptr);
void pool_free_all(PoolAllocator *allocator);
void *pool_alloc(PoolAllocator *allocator);
void pool_destroy(PoolAllocator *allocator);

#define pool_count_available(x) (x)->free_list->size

#endif // LIBFLINT_H_MEMORY
