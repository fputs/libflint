#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfqueue.h"

int main() {
    Queue *q = malloc(sizeof(Queue));
    queue_init(q, NULL);

    int a = 1, b = 2, c = 3;
    queue_enqueue(q, &a);
    queue_enqueue(q, &b);
    queue_enqueue(q, &c);
    ASSERT_EQ(q->size, 3);

    /* peek returns front without removing */
    int *p = queue_peek(q);
    ASSERT_EQ(*p, 1);
    ASSERT_EQ(q->size, 3);

    /* FIFO order */
    queue_dequeue(q, (void **)&p);
    ASSERT_EQ(*p, 1);
    queue_dequeue(q, (void **)&p);
    ASSERT_EQ(*p, 2);
    queue_dequeue(q, (void **)&p);
    ASSERT_EQ(*p, 3);
    ASSERT_EQ(q->size, 0);

    /* peek on empty queue */
    p = queue_peek(q);
    ASSERT_NULL(p);

    /* dequeue on empty queue */
    int ret = queue_dequeue(q, (void **)&p);
    ASSERT_EQ(ret, -1);

    queue_destroy(q);
    free(q);

    TEST_REPORT();
}
