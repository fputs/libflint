#ifndef LIBFLINT_STACK_H
#define LIBFLINT_STACK_H

#include "lflinkedlist.h"

/*
 * LIFO over a List. The wrapper type is what keeps ll_sort /
 * ll_ins_next from compiling against a Stack.
 */
typedef struct {
    List impl;
} Stack;

#define stack_size(s) ((s)->impl.size)
#define stack_is_empty(s) ((s)->impl.size == 0)

void stack_init(Stack *stack, void (*destroy)(void *data));

void stack_destroy(Stack *stack);

int stack_push(Stack *stack, const void *data);

void *stack_peek(Stack *stack);

int stack_pop(Stack *stack, void **data);

#endif
