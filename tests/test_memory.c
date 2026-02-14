#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfmemory.h"

int main() {
    ArenaAllocator *arena = malloc(sizeof(ArenaAllocator));
    arena_init(arena, 1024);

    int *i1 = arena_malloc(arena, sizeof(int));
    int *i2 = arena_malloc(arena, sizeof(int));

    *i1 = 1;
    *i2 = 2;

    ASSERT_LT(i1, i2);
    ASSERT_LT(*i1, *i2);

    long *l = arena_resize(arena, i1, sizeof(int), sizeof(long));
    ASSERT_EQ(*l, 1);

    unsigned char *char_test = arena_resize(arena, i2, sizeof(int), sizeof(unsigned char));
    ASSERT_EQ(*char_test, 2);

    /* Test in-place resize (most recent allocation) */
    int *last = arena_malloc(arena, sizeof(int));
    *last = 42;
    long *last_resized = arena_resize(arena, last, sizeof(int), sizeof(long));
    ASSERT_EQ((void *)last_resized, (void *)last);
    ASSERT_EQ(*last_resized, 42);

    /* Test resize returns NULL when arena is full */
    void *too_big = arena_resize(arena, last_resized, sizeof(long), 2048);
    ASSERT_NULL(too_big);

    /* Test arena_malloc returns NULL when full */
    void *full = arena_malloc(arena, 2048);
    ASSERT_NULL(full);

    /* Test arena_clear resets offsets */
    arena_clear(arena);
    int *after_clear = arena_malloc(arena, sizeof(int));
    ASSERT_NOT_NULL(after_clear);

    /* Test arena_resize_buf */
    arena_resize_buf(arena, 2048);
    void *big_alloc = arena_malloc(arena, 1500);
    ASSERT_NOT_NULL(big_alloc);

    arena_free(arena);
    free(arena);

    /* Test stack-allocated arena */
    ArenaAllocator stack_arena;
    arena_init(&stack_arena, 256);
    int *si = arena_malloc(&stack_arena, sizeof(int));
    ASSERT_NOT_NULL(si);
    *si = 99;
    ASSERT_EQ(*si, 99);
    arena_free(&stack_arena);

    PoolAllocator *pool = malloc(sizeof(PoolAllocator));
    pool_init(pool, 64, 16, LF_DEFAULT_ALIGNMENT);
    void *a = pool_alloc(pool);
    void *b = pool_alloc(pool);
    void *c = pool_alloc(pool);
    void *d = pool_alloc(pool);

    ASSERT_NOT_NULL(a);
    ASSERT_NOT_NULL(b);
    ASSERT_NOT_NULL(c);
    ASSERT_NOT_NULL(d);

    ASSERT_EQ(pool_count_available(pool), 0);
    pool_free(pool, d);
    ASSERT_EQ(pool_count_available(pool), 1);

    /* Test pool_free_all resets properly */
    pool_free_all(pool);
    size_t chunk_count = 64 / 16;
    ASSERT_EQ(pool_count_available(pool), chunk_count);

    /* Allocate again after free_all to verify no corruption */
    a = pool_alloc(pool);
    ASSERT_NOT_NULL(a);
    b = pool_alloc(pool);
    ASSERT_NOT_NULL(b);

    pool_destroy(pool);

    TEST_REPORT();
}
