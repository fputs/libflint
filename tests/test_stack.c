#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfstack.h"

static int destroyed_count;

static void counting_destroy(void *data) {
    (void)data;
    destroyed_count++;
}

int main() {
    /* basic push/pop */
    Stack *stack = malloc(sizeof(Stack));
    stack_init(stack, NULL);

    int a = 1, b = 2;
    stack_push(stack, &a);
    stack_push(stack, &b);
    ASSERT_EQ(stack_size(stack), 2);
    ASSERT_FALSE(stack_is_empty(stack));

    int *p = NULL;
    stack_pop(stack, (void **)&p);
    ASSERT_EQ(*p, 2);

    stack_pop(stack, (void **)&p);
    ASSERT_EQ(*p, 1);
    ASSERT_EQ(stack_size(stack), 0);
    ASSERT_TRUE(stack_is_empty(stack));

    stack_destroy(stack);
    free(stack);

    /* peek */
    stack = malloc(sizeof(Stack));
    stack_init(stack, NULL);

    ASSERT_NULL(stack_peek(stack));

    int x = 42;
    stack_push(stack, &x);
    p = stack_peek(stack);
    ASSERT_NOT_NULL(p);
    ASSERT_EQ(*p, 42);
    ASSERT_EQ(stack_size(stack), 1);

    stack_destroy(stack);
    free(stack);

    /* empty pop returns -1 */
    stack = malloc(sizeof(Stack));
    stack_init(stack, NULL);

    int ret = stack_pop(stack, (void **)&p);
    ASSERT_EQ(ret, -1);

    stack_destroy(stack);
    free(stack);

    /* return values from push/pop */
    stack = malloc(sizeof(Stack));
    stack_init(stack, NULL);

    ret = stack_push(stack, &a);
    ASSERT_EQ(ret, 0);
    ret = stack_pop(stack, (void **)&p);
    ASSERT_EQ(ret, 0);

    stack_destroy(stack);
    free(stack);

    /* destroy callback */
    stack = malloc(sizeof(Stack));
    destroyed_count = 0;
    stack_init(stack, counting_destroy);

    int *d1 = malloc(sizeof(int));
    int *d2 = malloc(sizeof(int));
    int *d3 = malloc(sizeof(int));
    *d1 = 10; *d2 = 20; *d3 = 30;
    stack_push(stack, d1);
    stack_push(stack, d2);
    stack_push(stack, d3);

    stack_destroy(stack);
    ASSERT_EQ(destroyed_count, 3);
    free(d1); free(d2); free(d3);
    free(stack);

    /* single-element push and pop */
    stack = malloc(sizeof(Stack));
    stack_init(stack, NULL);

    int v = 99;
    stack_push(stack, &v);
    ASSERT_EQ(stack_size(stack), 1);

    p = stack_peek(stack);
    ASSERT_EQ(*p, 99);

    ret = stack_pop(stack, (void **)&p);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(*p, 99);
    ASSERT_TRUE(stack_is_empty(stack));

    stack_destroy(stack);
    free(stack);

    TEST_REPORT();
}
