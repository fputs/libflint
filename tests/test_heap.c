#include "lfheap.h"
#include "lftest.h"
#include <stdlib.h>
static int cmp(const void *a, const void *b) {
    int x = a ? *(const int *)a : -1, y = b ? *(const int *)b : -1;
    return (x > y) - (x < y);
}
static int reverse(const void *a, const void *b) { return cmp(b, a); }
static int destroyed;
static void destroy(void *p) { destroyed++; free(p); }
int main(void) {
    LfHeap h;
    ASSERT_EQ(lf_heap_init(&h, NULL, NULL), -1);
    lf_heap_destroy(&h);
    ASSERT_EQ(lf_heap_init(&h, cmp, NULL), 0);
    void *v = &h;
    ASSERT_EQ(lf_heap_pop(&h, &v), 1); ASSERT_EQ(v, &h);
    ASSERT_EQ(lf_heap_reserve(&h, SIZE_MAX), -1);
    ASSERT_EQ(h.cap, 0);
    ASSERT_EQ(lf_heap_reserve(&h, 3), 0);
    int a[257];
    for (int i = 0; i < 257; i++) { a[i] = (i * 97) % 257; ASSERT_EQ(lf_heap_push(&h, &a[i]), 0); }
    ASSERT_EQ(lf_heap_push(&h, NULL), 0);
    ASSERT_EQ(lf_heap_peek(&h, &v), 0); ASSERT_NULL(v);
    ASSERT_EQ(lf_heap_pop(&h, &v), 0); ASSERT_NULL(v);
    for (int i = 0; i < 257; i++) {
        ASSERT_EQ(lf_heap_pop(&h, &v), 0); ASSERT_EQ(*(int *)v, i);
    }
    lf_heap_destroy(&h);
    ASSERT_EQ(lf_heap_init(&h, reverse, NULL), 0);
    for (int i = 0; i < 257; i++) ASSERT_EQ(lf_heap_push(&h, &a[i]), 0);
    for (int i = 256; i >= 0; i--) { ASSERT_EQ(lf_heap_pop(&h, &v), 0); ASSERT_EQ(*(int *)v, i); }
    ASSERT_EQ(lf_heap_push(&h, &a[0]), 0); ASSERT_EQ(lf_heap_push(&h, &a[0]), 0);
    ASSERT_EQ(lf_heap_pop(&h, &v), 0); ASSERT_EQ(lf_heap_pop(&h, &v), 0);
    lf_heap_destroy(&h);
    ASSERT_EQ(lf_heap_init(&h, cmp, destroy), 0);
    int *p = malloc(sizeof(*p)); *p = 1;
    ASSERT_EQ(lf_heap_push(&h, p), 0);
    ASSERT_EQ(lf_heap_pop(&h, &v), 0); ASSERT_EQ(destroyed, 0); free(v);
    ASSERT_EQ(lf_heap_push(&h, NULL), 0);
    lf_heap_clear(&h); ASSERT_EQ(destroyed, 1); ASSERT_TRUE(h.cap > 0);
    ASSERT_EQ(lf_heap_push(&h, NULL), 0);
    lf_heap_destroy(&h); ASSERT_EQ(destroyed, 2);
    lf_heap_destroy(&h);
    TEST_REPORT();
}
