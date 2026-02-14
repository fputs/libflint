#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfstack.h"

int main() {
    Stack *stack = malloc(sizeof(Stack));
    stack_init(stack, NULL);

    int a = 1, b = 2;
    stack_push(stack, &a);
    stack_push(stack, &b);
    ASSERT_EQ(stack->size, 2);

    int *p = NULL;
    stack_pop(stack, (void **) &p);
    ASSERT_EQ(*p, 2);

    stack_pop(stack, (void **) &p);
    ASSERT_EQ(*p, 1);
    ASSERT_EQ(stack->size, 0);

    stack_destroy(stack);
    free(stack);

    TEST_REPORT();
}
