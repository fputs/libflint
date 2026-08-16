#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfqueue.h"

static int destroyed_count;

static void counting_destroy(void *data) {
    (void)data;
    destroyed_count++;
}

int main() {
    /* basic enqueue/dequeue FIFO */
    Queue *q = malloc(sizeof(Queue));
    queue_init(q, NULL);

    int a = 1, b = 2, c = 3;
    queue_enqueue(q, &a);
    queue_enqueue(q, &b);
    queue_enqueue(q, &c);
    ASSERT_EQ(queue_size(q), 3);
    ASSERT_FALSE(queue_is_empty(q));

    int *p = queue_peek(q);
    ASSERT_EQ(*p, 1);
    ASSERT_EQ(queue_size(q), 3);

    queue_dequeue(q, (void **)&p);
    ASSERT_EQ(*p, 1);
    queue_dequeue(q, (void **)&p);
    ASSERT_EQ(*p, 2);
    queue_dequeue(q, (void **)&p);
    ASSERT_EQ(*p, 3);
    ASSERT_EQ(queue_size(q), 0);
    ASSERT_TRUE(queue_is_empty(q));

    /* peek on empty queue */
    p = queue_peek(q);
    ASSERT_NULL(p);

    /* dequeue on empty queue returns -1 */
    int ret = queue_dequeue(q, (void **)&p);
    ASSERT_EQ(ret, -1);

    queue_destroy(q);
    free(q);

    /* return values from enqueue/dequeue */
    q = malloc(sizeof(Queue));
    queue_init(q, NULL);

    ret = queue_enqueue(q, &a);
    ASSERT_EQ(ret, 0);
    ret = queue_dequeue(q, (void **)&p);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(*p, 1);

    queue_destroy(q);
    free(q);

    /* destroy callback */
    q = malloc(sizeof(Queue));
    destroyed_count = 0;
    queue_init(q, counting_destroy);

    int *d1 = malloc(sizeof(int));
    int *d2 = malloc(sizeof(int));
    *d1 = 10; *d2 = 20;
    queue_enqueue(q, d1);
    queue_enqueue(q, d2);

    queue_destroy(q);
    ASSERT_EQ(destroyed_count, 2);
    free(d1); free(d2);
    free(q);

    /* enqueue after drain */
    q = malloc(sizeof(Queue));
    queue_init(q, NULL);

    queue_enqueue(q, &a);
    queue_dequeue(q, (void **)&p);
    ASSERT_TRUE(queue_is_empty(q));

    queue_enqueue(q, &b);
    ASSERT_EQ(queue_size(q), 1);
    p = queue_peek(q);
    ASSERT_EQ(*p, 2);

    queue_dequeue(q, (void **)&p);
    ASSERT_EQ(*p, 2);
    ASSERT_TRUE(queue_is_empty(q));

    queue_destroy(q);
    free(q);

    /* single-element operations */
    q = malloc(sizeof(Queue));
    queue_init(q, NULL);

    int v = 77;
    queue_enqueue(q, &v);
    ASSERT_EQ(queue_size(q), 1);

    p = queue_peek(q);
    ASSERT_EQ(*p, 77);

    ret = queue_dequeue(q, (void **)&p);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(*p, 77);
    ASSERT_TRUE(queue_is_empty(q));

    queue_destroy(q);
    free(q);

    /* dequeue with NULL out-param must not drop the item */
    q = malloc(sizeof(Queue));
    queue_init(q, NULL);

    int keep = 5;
    queue_enqueue(q, &keep);
    ASSERT_EQ(queue_dequeue(q, NULL), -1);
    ASSERT_EQ(queue_size(q), 1);
    p = queue_peek(q);
    ASSERT_EQ(*p, 5);
    ASSERT_EQ(queue_dequeue(q, (void **)&p), 0);
    ASSERT_EQ(*p, 5);

    queue_destroy(q);
    free(q);

    TEST_REPORT();
}
