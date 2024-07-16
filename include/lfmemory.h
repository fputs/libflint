#ifndef LIBFLINT_H_MEMORY
#define LIBFLINT_H_MEMORY

#include <stddef.h>

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

#endif // LIBFLINT_H_MEMORY
