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

static uintptr_t align_forward_uintptr(const uintptr_t ptr, const uintptr_t align) {
    if (!is_power_of_two(align)) {
        // TODO: Error
    }
    uintptr_t p = ptr;
    const uintptr_t m = p & (align - 1);

    if (m != 0) {
        p += align - m;
    }
    return p;
}

static uintptr_t align_forward_size(const size_t ptr, const size_t align) {
    if (!is_power_of_two(align)) {
        // TODO: Error
    }
    uintptr_t p = ptr;
    const uintptr_t m = p & (align - 1);

    if (m != 0) {
        p += align - m;
    }
    return p;
}

static void *arena_malloc_align(ArenaAllocator *allocator, const size_t size, size_t align) {
    uintptr_t cur_ptr = (uintptr_t)allocator->buf + allocator->offset_cur;

    // Push forward to align, then change to relative offset
    uintptr_t offset = align_forward_uintptr(cur_ptr, align);
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

static void *arena_resize_align(ArenaAllocator *allocator, void *mem, const size_t old_sz, const size_t new_sz, size_t align) {
    unsigned char *old_mem = mem;
    if (!is_power_of_two(align)) {
        // TODO: Error handling
    }

    if (old_mem == NULL || old_sz == 0) {
        return arena_malloc_align(allocator, new_sz, align);
    }

    if (allocator->buf <= (unsigned char*)mem && (unsigned char*)mem < allocator->buf + allocator->buf_sz) {
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

void arena_resize_buf(ArenaAllocator *allocator, const size_t new_sz) {
    allocator->buf = realloc(allocator->buf, sizeof(unsigned char) * new_sz);
}

void *arena_malloc(ArenaAllocator *allocator, const size_t size) {
    return arena_malloc_align(allocator, size, LF_DEFAULT_ALIGNMENT);
}

void *arena_resize(ArenaAllocator *allocator, void *mem, const size_t old_sz, const size_t new_sz) {
    return arena_resize_align(allocator, mem, old_sz, new_sz, LF_DEFAULT_ALIGNMENT);
}

void pool_init(PoolAllocator *allocator, size_t buf_sz, size_t chunk_sz, size_t chunk_align) {
    if (allocator == NULL) {
        return;
    }

    allocator->buf = malloc(sizeof(unsigned char) * buf_sz);
    uintptr_t istart = (uintptr_t)allocator->buf;
    uintptr_t start = align_forward_uintptr(istart, chunk_align);
    allocator->buf_sz = buf_sz - (start - istart);

    allocator->chunk_size = align_forward_size(chunk_sz, chunk_align);
    if (allocator->chunk_size < sizeof(void *) || allocator->buf_sz < allocator->chunk_size) {
        //TODO: Handle error better
        return;
    }

    allocator->free_list = malloc(sizeof(List));
    ll_init(allocator->free_list, NULL);

    pool_free_all(allocator);
}

void pool_free(PoolAllocator *allocator, void *ptr) {
    ListNode *node = NULL;
    const void *start = allocator->buf;
    const void *end = &allocator->buf[allocator->buf_sz];

    if (ptr == NULL) {
        return;
    }

    if (!(start <= ptr && ptr < end)) {
        // TODO: Handle error better
        return;
    }

    ll_ins_next(allocator->free_list, allocator->free_list->tail, ptr);
}

void pool_free_all(PoolAllocator *allocator) {
    size_t chunk_count = allocator->buf_sz / allocator->chunk_size;
    for (size_t i = 0; i < chunk_count; ++i) {
        ll_ins_next(allocator->free_list, allocator->free_list->head, &allocator->buf[i * allocator->chunk_size]);
    }
}

void *pool_alloc(PoolAllocator *allocator) {
    ListNode *node = allocator->free_list->head;
    if (node == NULL) {
        // TODO: Handle error better
        return NULL;
    }

    void *tmp;
    ll_remove(allocator->free_list, allocator->free_list->head, &tmp);
    return memset(tmp, 0, allocator->chunk_size);
}

void pool_destroy(PoolAllocator *allocator) {
    ll_destroy(allocator->free_list);
    free(allocator->free_list);
    free(allocator->buf);
    free(allocator);
}
