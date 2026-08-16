LIFO stack of `void *`. List functions do not compile against a `Stack`.

## Structs

```c
typedef struct {
    List impl;
} Stack;
```

The top of the stack is the most recently pushed element.

## Functions

### stack_init

Initializes the stack. The user is responsible for creating the initial `Stack` structure and freeing memory with `stack_destroy()`. `destroy` is a deallocation function for the members of `Stack`, pass `NULL` if the memory is stack-allocated

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

Destroys the nodes inside a `Stack` and calls the deallocation function on the data if one was provided. Does not destroy the stack itself, that is left up to the user.

```c
void stack_destroy(Stack *stack);
```

### stack_push

Push a new element onto the top of the stack. Returns `0` on success, `-1` on error.

```c
int stack_push(Stack *stack, const void *data);

/* Usage */
int x = 42;
stack_push(stack, &x);
```

### stack_peek

Returns a `void *` to the top element of the stack. Does not remove the element. Returns `NULL` if the stack is empty or the stored payload is `NULL`.

```c
void *stack_peek(Stack *stack);

/* Usage */
// stack: 1 2 3
int *t = (int*)stack_peek(stack);
assert(*t == 3);
// stack: 1 2 3
```

### stack_pop

Pops the top element of the stack and stores its data in `data`. Returns `0` on success, `-1` on error. `data` must not be `NULL`; a null out-param does not drop the item.

```c
int stack_pop(Stack *stack, void **data);

/* Usage */
// stack: 1 2 3
int *t;
stack_pop(stack, (void **)&t);
assert(*t == 3);
// stack: 1 2
```

## Macros

### stack_size

Returns the number of elements in the stack.

```c
#define stack_size(s) ((s)->impl.size)
```

### stack_is_empty

Returns true if the stack has no elements.

```c
#define stack_is_empty(s) ((s)->impl.size == 0)
```
