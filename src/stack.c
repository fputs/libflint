#include "stack.h"

void stack_init(struct Stack* stack, void (*destroy)(void* data)) {
    ll_init(stack, destroy);
}

void stack_destroy(struct Stack* stack) {
    ll_destroy(stack);
}

int stack_push(struct Stack* stack, void *data) {
    if (stack->size == 0) {
        return ll_ins_next(stack, NULL, data);
    } else {
        return ll_ins_next(stack, stack->tail, data);
    }
}

void *stack_peek(struct Stack *stack) {
    return stack->tail == NULL ? NULL : stack->tail->data;
}

int stack_pop(struct Stack *stack, void **data) {
    return ll_remove(stack, stack->tail, data);
}
