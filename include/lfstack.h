#ifndef LIBFLINT_STACK_H
#define LIBFLINT_STACK_H

#include "lflinkedlist.h"

#define Stack List

void stack_init(Stack *stack, void (*destroy)(void *data));

void stack_destroy(Stack *stack);

int stack_push(Stack *stack, void *data);

void *stack_peek(Stack *stack);

int stack_pop(Stack *stack, void **data);

#endif
