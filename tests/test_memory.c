#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfmemory.h"

int main() {
    ArenaAllocator *arena = malloc(sizeof(ArenaAllocator));
    ASSERT_EQ(arena_init(arena, 1024), 0);

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
    ASSERT_EQ(arena_resize_buf(arena, 2048), 0);
    void *big_alloc = arena_malloc(arena, 1500);
    ASSERT_NOT_NULL(big_alloc);

    /* Test arena_resize_buf rejects shrink below offset_cur */
    ASSERT_EQ(arena_resize_buf(arena, 1), -1);

    /* Test arena_init returns -1 for NULL */
    ASSERT_EQ(arena_init(NULL, 64), -1);

    arena_free(arena);
    free(arena);

    /* Test stack-allocated arena */
    ArenaAllocator stack_arena;
    ASSERT_EQ(arena_init(&stack_arena, 256), 0);
    int *si = arena_malloc(&stack_arena, sizeof(int));
    ASSERT_NOT_NULL(si);
    *si = 99;
    ASSERT_EQ(*si, 99);
    arena_free(&stack_arena);

    /* Pool tests */
    PoolAllocator *pool = malloc(sizeof(PoolAllocator));
    ASSERT_EQ(pool_init(pool, 64, 16, LF_DEFAULT_ALIGNMENT), 0);
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
    size_t chunk_count = pool->buf_sz / pool->chunk_size;
    ASSERT_EQ(pool_count_available(pool), chunk_count);

    /* Allocate again after free_all to verify no corruption */
    a = pool_alloc(pool);
    ASSERT_NOT_NULL(a);
    b = pool_alloc(pool);
    ASSERT_NOT_NULL(b);

    pool_destroy(pool);
    free(pool);

    /* Pool exhaustion: allocating more chunks than capacity returns NULL */
    PoolAllocator pool_ex;
    ASSERT_EQ(pool_init(&pool_ex, 64, 16, LF_DEFAULT_ALIGNMENT), 0);
    size_t ex_count = pool_ex.buf_sz / pool_ex.chunk_size;
    for (size_t i = 0; i < ex_count; ++i) {
        ASSERT_NOT_NULL(pool_alloc(&pool_ex));
    }
    ASSERT_NULL(pool_alloc(&pool_ex));
    pool_destroy(&pool_ex);

    /* Pool free with bad pointers (outside buffer range) */
    PoolAllocator pool_bad;
    ASSERT_EQ(pool_init(&pool_bad, 64, 16, LF_DEFAULT_ALIGNMENT), 0);
    size_t before = pool_count_available(&pool_bad);
    int dummy = 0;
    pool_free(&pool_bad, &dummy);           /* pointer outside pool */
    pool_free(&pool_bad, NULL);             /* NULL pointer */
    ASSERT_EQ(pool_count_available(&pool_bad), before); /* count unchanged */
    pool_destroy(&pool_bad);

    /* Pool free + realloc cycle */
    PoolAllocator pool_cycle;
    ASSERT_EQ(pool_init(&pool_cycle, 64, 16, LF_DEFAULT_ALIGNMENT), 0);
    /* drain all chunks first */
    size_t cycle_count = pool_cycle.buf_sz / pool_cycle.chunk_size;
    for (size_t i = 0; i < cycle_count; ++i) {
        pool_alloc(&pool_cycle);
    }
    ASSERT_EQ(pool_count_available(&pool_cycle), 0);
    /* free one, then alloc -- must get it back */
    void *p1 = &pool_cycle.buf[pool_cycle.aligned_start]; /* first chunk addr */
    pool_free(&pool_cycle, p1);
    void *p2 = pool_alloc(&pool_cycle);
    ASSERT_NOT_NULL(p2);
    ASSERT_EQ(p1, p2); /* should reuse the freed chunk */
    pool_destroy(&pool_cycle);

    /* Pool init with bad params */
    PoolAllocator pool_bp;
    ASSERT_EQ(pool_init(&pool_bp, 4, 16, LF_DEFAULT_ALIGNMENT), -1);  /* buf too small */
    ASSERT_EQ(pool_init(NULL, 64, 16, LF_DEFAULT_ALIGNMENT), -1);     /* NULL allocator */
    ASSERT_EQ(pool_init(&pool_bp, 64, 16, 3), -1);                   /* not power of two */
    ASSERT_EQ(pool_init(&pool_bp, 64, 16, 0), -1);
    ASSERT_EQ(pool_init(&pool_bp, 64, 16, (size_t)1 << (sizeof(size_t) * 8 - 1)), -1);
    ASSERT_EQ(pool_init(&pool_bp, 64, 0, LF_DEFAULT_ALIGNMENT), -1);

    /* Double-free does not hand the same chunk to two allocs */
    PoolAllocator pool_df;
    ASSERT_EQ(pool_init(&pool_df, 64, 16, LF_DEFAULT_ALIGNMENT), 0);
    size_t df_n = pool_count_available(&pool_df);
    void *df_x = pool_alloc(&pool_df);
    ASSERT_NOT_NULL(df_x);
    pool_free(&pool_df, df_x);
    ASSERT_EQ(pool_count_available(&pool_df), df_n);
    pool_free(&pool_df, df_x);
    ASSERT_EQ(pool_count_available(&pool_df), df_n);
    void *df_y = pool_alloc(&pool_df);
    void *df_z = pool_alloc(&pool_df);
    ASSERT_NOT_NULL(df_y);
    ASSERT_NOT_NULL(df_z);
    ASSERT_TRUE(df_y != df_z);
    pool_destroy(&pool_df);

    /* arena save/restore */
    ArenaAllocator save_arena;
    arena_init(&save_arena, 512);

    int *perm1 = arena_malloc(&save_arena, sizeof(int));
    *perm1 = 100;
    int *perm2 = arena_malloc(&save_arena, sizeof(int));
    *perm2 = 200;

    /* save, do temp work, restore */
    ASSERT_EQ(arena_save(&save_arena), 0);
    size_t saved_offset = save_arena.offset_cur;

    int *tmp1 = arena_malloc(&save_arena, sizeof(int));
    *tmp1 = 999;
    int *tmp2 = arena_malloc(&save_arena, sizeof(int));
    *tmp2 = 888;
    ASSERT_TRUE(save_arena.offset_cur > saved_offset);

    arena_restore(&save_arena);
    ASSERT_EQ(save_arena.offset_cur, saved_offset);

    /* permanent data still intact */
    ASSERT_EQ(*perm1, 100);
    ASSERT_EQ(*perm2, 200);

    /* new alloc reuses temp space */
    int *perm3 = arena_malloc(&save_arena, sizeof(int));
    *perm3 = 300;
    ASSERT_NOT_NULL(perm3);

    /* nested save/restore */
    arena_clear(&save_arena);
    int *outer = arena_malloc(&save_arena, sizeof(int));
    *outer = 1;

    ASSERT_EQ(arena_save(&save_arena), 0);
    size_t outer_offset = save_arena.offset_cur;

    int *mid = arena_malloc(&save_arena, sizeof(int));
    *mid = 2;

    ASSERT_EQ(arena_save(&save_arena), 0);
    size_t inner_offset = save_arena.offset_cur;

    int *inner = arena_malloc(&save_arena, sizeof(int));
    *inner = 3;

    arena_restore(&save_arena);
    ASSERT_EQ(save_arena.offset_cur, inner_offset);

    arena_restore(&save_arena);
    ASSERT_EQ(save_arena.offset_cur, outer_offset);

    ASSERT_EQ(*outer, 1);

    /* restore on empty stack is a no-op */
    arena_restore(&save_arena);
    ASSERT_EQ(save_arena.offset_cur, outer_offset);

    arena_free(&save_arena);

    /* caller-provided backing store */
    unsigned char storage[256];
    memset(storage, 0xaa, sizeof storage);
    ArenaAllocator buf_arena;
    ASSERT_EQ(arena_init_buf(&buf_arena, storage, sizeof storage), 0);
    ASSERT_EQ(buf_arena.owns_buf, 0);
    int *bp = arena_malloc(&buf_arena, sizeof(int));
    ASSERT_NOT_NULL(bp);
    *bp = 11;
    ASSERT_TRUE((unsigned char *)bp >= storage);
    ASSERT_TRUE((unsigned char *)bp < storage + sizeof storage);
    ASSERT_EQ(arena_resize_buf(&buf_arena, 512), -1);
    arena_free(&buf_arena);
    ASSERT_NULL(buf_arena.buf);
    ASSERT_EQ(buf_arena.owns_buf, 0);
    ASSERT_EQ(*bp, 11);

    ASSERT_EQ(arena_init_buf(NULL, storage, sizeof storage), -1);
    ASSERT_EQ(arena_init_buf(&buf_arena, NULL, sizeof storage), -1);
    ASSERT_EQ(arena_init_buf(&buf_arena, storage, 0), -1);

    /* size wrap: huge request must fail closed, not pass the bounds check */
    ArenaAllocator wrap_arena;
    ASSERT_EQ(arena_init(&wrap_arena, 64), 0);
    ASSERT_NULL(arena_malloc(&wrap_arena, SIZE_MAX));
    int *ok = arena_malloc(&wrap_arena, sizeof(int));
    ASSERT_NOT_NULL(ok);
    *ok = 7;
    ASSERT_NULL(arena_resize(&wrap_arena, ok, sizeof(int), SIZE_MAX));
    ASSERT_EQ(*ok, 7);
    arena_free(&wrap_arena);

    TEST_REPORT();
}
