#ifndef LIBFLINT_STACK_H
#define LIBFLINT_STACK_H

#include "linkedlist.h"

#define Stack List

void stack_init(struct Stack* stack, void (*destroy)(void* data));
void stack_destroy(struct Stack* stack);
int stack_push(struct Stack* stack, void *data);
void *stack_peek(struct Stack *stack);
int stack_pop(struct Stack *stack, void **data);

#endif
