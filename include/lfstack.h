#ifndef LIBFLINT_STACK_H
#define LIBFLINT_STACK_H

#include "lflinkedlist.h"

#define Stack List
#define stack_size(s) ((s)->size)
#define stack_is_empty(s) ((s)->size == 0)

void stack_init(Stack *stack, void (*destroy)(void *data));

void stack_destroy(Stack *stack);

int stack_push(Stack *stack, void *data);

void *stack_peek(Stack *stack);

int stack_pop(Stack *stack, void **data);

#endif
