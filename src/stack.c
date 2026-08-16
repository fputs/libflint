#include "lfstack.h"

void stack_init(Stack *stack, void (*destroy)(void *data)) {
    ll_init(&stack->impl, destroy, NULL);
}

void stack_destroy(Stack *stack) {
    ll_destroy(&stack->impl);
}

int stack_push(Stack *stack, const void *data) {
    return ll_push_tail(&stack->impl, data);
}

void *stack_peek(Stack *stack) {
    return stack->impl.tail == NULL ? NULL : stack->impl.tail->data;
}

int stack_pop(Stack *stack, void **data) {
    return ll_pop_tail(&stack->impl, data);
}
