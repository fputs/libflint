#include <lfmath.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "lfmemory.h"

#define arena_sz(a) (a)->buf_sz

void arena_init(ArenaAllocator *allocator, size_t buf_sz) {
    if (allocator == NULL) {
        return;
    }

    allocator->buf = malloc(sizeof(unsigned char) * buf_sz);
    allocator->buf_sz = buf_sz;
    allocator->offset_cur = 0;
    allocator->offset_prev = 0;
}

void arena_free(ArenaAllocator *allocator) {
    free(allocator->buf);
    free(allocator);
}

void arena_clear(ArenaAllocator *allocator) {
    allocator->offset_cur = 0;
    allocator->offset_prev = 0;
}

static uintptr_t align_forward(uintptr_t ptr, size_t align) {
    uintptr_t p, a, m;
    if (!is_power_of_two(align)) {
        // TODO: Error
    }
    p = ptr;
    a = (uintptr_t)align;
    m = p & (a - 1);

    if (m != 0) {
        p += a - m;
    }
    return p;
}

static void *arena_malloc_align(ArenaAllocator *allocator, size_t size, size_t align) {
    uintptr_t cur_ptr = (uintptr_t)allocator->buf + (uintptr_t)allocator->offset_cur;

    // Push forward to align, then change to relative offset
    uintptr_t offset = align_forward(cur_ptr, align);
    offset -= (uintptr_t)allocator->buf;

    if (offset + size <= allocator->buf_sz) {
        void *ptr = &allocator->buf[offset];
        allocator->offset_prev = offset;
        allocator->offset_cur = offset + size;
        memset(ptr, 0, size);
        return ptr;
    }

    // Arena is full
    return NULL;
}

static void *arena_resize_align(ArenaAllocator *allocator, void *mem, size_t old_sz, size_t new_sz, size_t align) {
    unsigned char *old_mem = (unsigned char *)mem;
    if (!is_power_of_two(align)) {
        // TODO: Error handling
    }

    if (old_mem == NULL || old_sz == 0) {
        return arena_malloc_align(allocator, new_sz, align);
    }

    if (allocator->buf <= mem && mem < allocator->buf + allocator->buf_sz) {
        if (allocator->buf + allocator->offset_prev == old_mem) {
            allocator->offset_cur = allocator->offset_prev + new_sz;
            if (new_sz > old_sz) {
                // Zero out memory
                memset(&allocator->buf[allocator->offset_cur], 0, new_sz - old_sz);
            }
            return old_mem;
        }

        void *new_mem = arena_malloc_align(allocator, new_sz, align);
        size_t copy_size = old_sz < new_sz ? old_sz : new_sz;
        memmove(new_mem, old_mem, copy_size);
        return new_mem;
    }

    return NULL;
}

void arena_resize_buf(ArenaAllocator *allocator, size_t new_sz) {
    allocator->buf = realloc(allocator->buf, sizeof(unsigned char) * new_sz);
}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void*))
#endif // DEFAULT_ALIGNMENT

void *arena_malloc(ArenaAllocator *allocator, size_t size) {
    return arena_malloc_align(allocator, size, DEFAULT_ALIGNMENT);
}

void *arena_resize(ArenaAllocator *allocator, void *mem, size_t old_sz, size_t new_sz) {
    return arena_resize_align(allocator, mem, old_sz, new_sz, DEFAULT_ALIGNMENT);
}

