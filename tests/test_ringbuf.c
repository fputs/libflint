#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfringbuf.h"

static int rb_dropped;

static void counting_free(void *data) {
    free(data);
    rb_dropped++;
}

static void test_stackalloc() {
    /* Basic insertion and size */
    RingBuffer *rb = malloc(sizeof(RingBuffer));
    rb_init(rb, 3, NULL);

    ASSERT_TRUE(rb_is_empty(rb));

    int i = 1;
    int j = 2;
    int k = 4;
    rb_push(rb, (void *) &i);
    rb_push(rb, (void *) &j);
    rb_push(rb, (void *) &k);

    ASSERT_EQ(rb_count(rb), 3);
    ASSERT_TRUE(rb_is_full(rb));

    /* Push when full */
    int x = 99;
    int ret = rb_push(rb, (void *) &x);
    ASSERT_EQ(ret, -1);

    /* Normal pop and peek */
    void *data;
    ret = rb_pop(rb, &data);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(*(int *)data, 1);
    ASSERT_EQ(rb_count(rb), 2);

    data = rb_peek(rb);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int *)data, 2);
    ASSERT_EQ(rb_count(rb), 2);

    rb_clear(rb);
    ASSERT_TRUE(rb_is_empty(rb));

    /* test empty */
    rb_destroy(rb);
    free(rb);
    rb = NULL;

}

static void test_heapalloc() {
    RingBuffer *rb = malloc(sizeof(RingBuffer));
    rb_init(rb, 3, free);
    void *data;

    ASSERT_TRUE(rb_is_empty(rb));

    int *pi = malloc(sizeof(int));
    int *pj = malloc(sizeof(int));
    int *pk = malloc(sizeof(int));
    *pi = 1;
    *pj = 2;
    *pk = 4;
    rb_push(rb, (void *) pi);
    rb_push(rb, (void *) pj);
    rb_push(rb, (void *) pk);

    ASSERT_EQ(rb_count(rb), 3);
    ASSERT_TRUE(rb_is_full(rb));

    /* Normal pop and peek */
    int ret = rb_pop(rb, &data);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(*(int *)data, 1);
    ASSERT_EQ(rb_count(rb), 2);
    free(data); // frees pi

    data = rb_peek(rb);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int *)data, 2);
    ASSERT_EQ(rb_count(rb), 2);

    rb_destroy(rb);
    free(rb);
    rb = NULL;
}

static void test_wrap() {
    RingBuffer *rb = malloc(sizeof(RingBuffer));
    rb_init(rb, 3, NULL);

    int a = 1;
    int b = 2;
    int c = 4;
    rb_push(rb, (void *) &a);
    rb_push(rb, (void *) &b);
    rb_push(rb, (void *) &c);

    ASSERT_TRUE(rb_is_full(rb));

    /* pop a */
    void *data;
    int ret = rb_pop(rb, &data);
    ASSERT_EQ(ret, 0);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int *)data, 1);
    ASSERT_EQ(rb_count(rb), 2);

    /* pop b */
    ret = rb_pop(rb, &data);
    ASSERT_EQ(ret, 0);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int *)data, 2);
    ASSERT_EQ(rb_count(rb), 1);

    int d = 10;
    ret = rb_push(rb, (void *) &d);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(rb_count(rb), 2);

    int e = 20;
    ret = rb_push(rb, (void *) &e);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(rb_count(rb), 3);

    /* pop c */
    ret = rb_pop(rb, &data);
    ASSERT_EQ(ret, 0);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int *)data, 4);

    /* pop d */
    ret = rb_pop(rb, &data);
    ASSERT_EQ(ret, 0);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int *)data, 10);

    /* pop e */
    ret = rb_pop(rb, &data);
    ASSERT_EQ(ret, 0);
    ASSERT_NOT_NULL(data);
    ASSERT_EQ(*(int *)data, 20);
    ASSERT_TRUE(rb_is_empty(rb));

    rb_destroy(rb);
    free(rb);
    rb = NULL;
}

static void test_init_and_pop_null(void) {
    ASSERT_EQ(rb_init(NULL, 4, NULL), -1);

    RingBuffer rb;
    ASSERT_EQ(rb_init(&rb, 0, NULL), -1);
    ASSERT_EQ(rb.capacity, (size_t)0);
    ASSERT_NULL(rb.buf);
    rb_destroy(&rb);

    ASSERT_EQ(rb_init(&rb, 2, NULL), 0);
    int a = 1, b = 2;
    ASSERT_EQ(rb_push(&rb, &a), 0);
    ASSERT_EQ(rb_push(&rb, &b), 0);
    ASSERT_EQ(rb_pop(&rb, NULL), -1);
    ASSERT_EQ(rb_count(&rb), 2);
    ASSERT_EQ(*(int *)rb_peek(&rb), 1);
    rb_destroy(&rb);
}

static void test_force_push(void) {
    RingBuffer rb;
    ASSERT_EQ(rb_init(&rb, 2, NULL), 0);
    int a = 1, b = 2, c = 3;
    ASSERT_EQ(rb_force_push(&rb, &a), 0);
    ASSERT_EQ(rb_force_push(&rb, &b), 0);
    ASSERT_TRUE(rb_is_full(&rb));
    ASSERT_EQ(rb_force_push(&rb, &c), 0);
    ASSERT_EQ(rb_count(&rb), 2);
    ASSERT_EQ(*(int *)rb_peek(&rb), 2);
    void *data;
    ASSERT_EQ(rb_pop(&rb, &data), 0);
    ASSERT_EQ(*(int *)data, 2);
    ASSERT_EQ(rb_pop(&rb, &data), 0);
    ASSERT_EQ(*(int *)data, 3);
    rb_destroy(&rb);

    ASSERT_EQ(rb_force_push(NULL, &a), -1);

    rb_dropped = 0;
    RingBuffer own;
    ASSERT_EQ(rb_init(&own, 1, counting_free), 0);
    int *p = malloc(sizeof(int));
    int *q = malloc(sizeof(int));
    *p = 1;
    *q = 2;
    ASSERT_EQ(rb_force_push(&own, p), 0);
    ASSERT_EQ(rb_force_push(&own, q), 0);
    ASSERT_EQ(rb_dropped, 1);
    rb_destroy(&own);
    ASSERT_EQ(rb_dropped, 2);
}

int main() {
    test_stackalloc();
    test_heapalloc();
    test_wrap();
    test_init_and_pop_null();
    test_force_push();
    TEST_REPORT();
}
