#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "lfmemory.h"

static int is_pow2_uptr(uintptr_t n) {
    return n != 0 && (n & (n - 1)) == 0;
}

int arena_init(ArenaAllocator *allocator, size_t buf_sz) {
    if (allocator == NULL) {
        return -1;
    }

    allocator->buf = malloc(sizeof(unsigned char) * buf_sz);
    if (allocator->buf == NULL) {
        return -1;
    }
    allocator->buf_sz = buf_sz;
    allocator->offset_cur = 0;
    allocator->offset_prev = 0;
    allocator->save_count = 0;
    return 0;
}

void arena_free(ArenaAllocator *allocator) {
    if (allocator == NULL) {
        return;
    }
    free(allocator->buf);
    allocator->buf = NULL;
    allocator->buf_sz = 0;
    allocator->offset_cur = 0;
    allocator->offset_prev = 0;
    allocator->save_count = 0;
}

void arena_clear(ArenaAllocator *allocator) {
    allocator->offset_cur = 0;
    allocator->offset_prev = 0;
    allocator->save_count = 0;
}

int arena_save(ArenaAllocator *allocator) {
    if (allocator->save_count >= LF_ARENA_MAX_SAVE_POINTS) {
        return -1;
    }
    allocator->save_stack[allocator->save_count++] = allocator->offset_cur;
    return 0;
}

void arena_restore(ArenaAllocator *allocator) {
    if (allocator->save_count == 0) {
        return;
    }
    allocator->offset_cur = allocator->save_stack[--allocator->save_count];
    allocator->offset_prev = allocator->offset_cur;
}

static uintptr_t align_forward(const uintptr_t ptr, const uintptr_t align) {
    if (!is_pow2_uptr(align)) {
        return 0;
    }
    uintptr_t p = ptr;
    const uintptr_t m = p & (align - 1);

    if (m != 0) {
        uintptr_t add = align - m;
        if (p > UINTPTR_MAX - add) {
            return 0;
        }
        p += add;
    }
    return p;
}

static void *arena_malloc_align(ArenaAllocator *allocator, const size_t size, size_t align) {
    uintptr_t cur_ptr = (uintptr_t)allocator->buf + allocator->offset_cur;

    // Push forward to align, then change to relative offset
    uintptr_t offset = align_forward(cur_ptr, align);
    offset -= (uintptr_t)allocator->buf;

    if (offset <= allocator->buf_sz && size <= allocator->buf_sz - offset) {
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
    if (!is_pow2_uptr(align)) {
        return NULL;
    }

    if (old_mem == NULL || old_sz == 0) {
        return arena_malloc_align(allocator, new_sz, align);
    }

    if (allocator->buf <= (unsigned char*)mem && (unsigned char*)mem < allocator->buf + allocator->buf_sz) {
        if (allocator->buf + allocator->offset_prev == old_mem) {
            if (allocator->offset_prev > allocator->buf_sz ||
                new_sz > allocator->buf_sz - allocator->offset_prev) {
                return NULL;
            }
            allocator->offset_cur = allocator->offset_prev + new_sz;
            if (new_sz > old_sz) {
                memset(&allocator->buf[allocator->offset_prev + old_sz], 0, new_sz - old_sz);
            }
            return old_mem;
        }

        void *new_mem = arena_malloc_align(allocator, new_sz, align);
        if (new_mem == NULL) {
            return NULL;
        }
        size_t copy_size = old_sz < new_sz ? old_sz : new_sz;
        memmove(new_mem, old_mem, copy_size);
        return new_mem;
    }

    return NULL;
}

int arena_resize_buf(ArenaAllocator *allocator, const size_t new_sz) {
    if (new_sz < allocator->offset_cur) {
        return -1;
    }
    unsigned char *new_buf = realloc(allocator->buf, sizeof(unsigned char) * new_sz);
    if (new_buf == NULL) {
        return -1;
    }
    allocator->buf = new_buf;
    allocator->buf_sz = new_sz;
    return 0;
}

void *arena_malloc(ArenaAllocator *allocator, const size_t size) {
    return arena_malloc_align(allocator, size, LF_DEFAULT_ALIGNMENT);
}

void *arena_resize(ArenaAllocator *allocator, void *mem, const size_t old_sz, const size_t new_sz) {
    return arena_resize_align(allocator, mem, old_sz, new_sz, LF_DEFAULT_ALIGNMENT);
}

static void pool_zero(PoolAllocator *allocator) {
    allocator->buf = NULL;
    allocator->buf_sz = 0;
    allocator->chunk_size = 0;
    allocator->aligned_start = 0;
    allocator->free_count = 0;
    allocator->free_head = NULL;
}

int pool_init(PoolAllocator *allocator, size_t buf_sz, size_t chunk_sz, size_t chunk_align) {
    if (allocator == NULL || buf_sz == 0 || chunk_sz == 0) {
        return -1;
    }
    if (!is_pow2_uptr(chunk_align)) {
        return -1;
    }

    unsigned char *buf = malloc(buf_sz);
    if (buf == NULL) {
        return -1;
    }

    uintptr_t istart = (uintptr_t)buf;
    uintptr_t start = align_forward(istart, chunk_align);
    if (start == 0 || start < istart) {
        free(buf);
        return -1;
    }

    size_t aligned_start = (size_t)(start - istart);
    if (aligned_start >= buf_sz) {
        free(buf);
        return -1;
    }

    size_t usable = buf_sz - aligned_start;
    uintptr_t aligned_chunk = align_forward((uintptr_t)chunk_sz, chunk_align);
    if (aligned_chunk == 0 || aligned_chunk < sizeof(void *) ||
        aligned_chunk > usable) {
        free(buf);
        return -1;
    }

    allocator->buf = buf;
    allocator->aligned_start = aligned_start;
    allocator->buf_sz = usable;
    allocator->chunk_size = (size_t)aligned_chunk;
    allocator->free_head = NULL;
    allocator->free_count = 0;
    pool_free_all(allocator);
    return 0;
}

void pool_free(PoolAllocator *allocator, void *ptr) {
    if (allocator == NULL || allocator->buf == NULL || ptr == NULL) {
        return;
    }

    unsigned char *start = allocator->buf + allocator->aligned_start;
    unsigned char *end = start + allocator->buf_sz;
    unsigned char *p = ptr;

    if (p < start || p >= end) {
        return;
    }
    size_t off = (size_t)(p - start);
    if (allocator->chunk_size == 0 || off % allocator->chunk_size != 0) {
        return;
    }

    size_t seen = 0;
    for (void *n = allocator->free_head; n != NULL; n = *(void **)n) {
        if (n == ptr || seen >= allocator->free_count) {
            return;
        }
        seen++;
    }

    *(void **)ptr = allocator->free_head;
    allocator->free_head = ptr;
    allocator->free_count++;
}

void pool_free_all(PoolAllocator *allocator) {
    if (allocator == NULL || allocator->buf == NULL || allocator->chunk_size == 0) {
        return;
    }

    allocator->free_head = NULL;
    allocator->free_count = 0;

    size_t chunk_count = allocator->buf_sz / allocator->chunk_size;
    for (size_t i = chunk_count; i > 0; --i) {
        void *chunk = &allocator->buf[allocator->aligned_start +
                                      (i - 1) * allocator->chunk_size];
        *(void **)chunk = allocator->free_head;
        allocator->free_head = chunk;
        allocator->free_count++;
    }
}

void *pool_alloc(PoolAllocator *allocator) {
    if (allocator == NULL || allocator->free_head == NULL) {
        return NULL;
    }

    void *ptr = allocator->free_head;
    allocator->free_head = *(void **)ptr;
    allocator->free_count--;
    return memset(ptr, 0, allocator->chunk_size);
}

void pool_destroy(PoolAllocator *allocator) {
    if (allocator == NULL) {
        return;
    }
    free(allocator->buf);
    pool_zero(allocator);
}
