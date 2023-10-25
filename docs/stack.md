# stack

LIFO (last in first out) stack structure

## Structs

`Stack` is a linked list with stack-based operations available

```c
#define Stack List
```

## Functions

### stack_init

Intializes the stack. The user is responsible for creating the initial `Stack` structure and freeing memory with `stack_destroy()`. `destroy` is a deallocation function for the members of `Stack`, pass `NULL` if the memory is stack-allocated

```c
void stack_init(Stack *stack, void (*destroy)(void *data));

/* Usage */
Stack *stack = malloc(sizeof(Stack));

// Pass NULL for stack-allocated memory
stack_init(stack, NULL);

// Pass free or a custom freeing function for heap allocated memory
stack_init(stack, free);
```

### stack_destroy

Destroys the nodes inside a `Stack` and calls the deallocation funciton on the data if one was provided. Does not destroy the list itself, that is left up to the user.

```c
void stack_destroy(Stack *stack);
```

### stack_push

Push a new element onto the top of the stack

```c
int stack_push(Stack *stack, void *data);
```

### stack_peek

Returns a `void *` to the top element of the stack. Does not remove the element

```c
void *stack_peek(Stack *stack);

/* Usage */
// stack: 1 2 3
int *t = (int*)stack_peek(stack);
assert(*t == 3);
// stack: 1 2 3
```

### stack_pop

Returns a `void *` to the top element of the stack and removes the element

```c
int stack_pop(Stack *stack, void **data);

/* Usage */
// stack: 1 2 3
int *t = (int*)stack_peek(stack);
assert(*t == 3);
// stack: 1 2
```