FIFO (first in first out) queue of `void *`. Backed by a `List`, but `Queue` is a real wrapper type so list operations do not compile against it.

## Structs

```c
typedef struct {
    List impl;
} Queue;
```

Enqueue is `ll_push_tail`. Dequeue and peek are at the head.

## Functions

### queue_init

Initializes the queue. The user is responsible for creating the initial `Queue` structure and freeing memory with `queue_destroy()`. `destroy` is a deallocation function for the members of `Queue`, pass `NULL` if the memory is stack-allocated

```c
void queue_init(Queue *queue, void (*destroy)(void *data));

/* Usage */
Queue *queue = malloc(sizeof(Queue));

// Pass NULL for stack-allocated memory
queue_init(queue, NULL);

// Pass free or a custom freeing function for heap allocated memory
queue_init(queue, free);
```

### queue_destroy

Destroys the nodes inside a `Queue` and calls the deallocation function on the data if one was provided. Does not destroy the queue itself, that is left up to the user.

```c
void queue_destroy(Queue *queue);
```

### queue_enqueue

Adds an element to the back of the queue. Returns `0` on success, `-1` on error.

```c
int queue_enqueue(Queue *queue, const void *data);

/* Usage */
int a = 1;
queue_enqueue(queue, &a);
```

### queue_dequeue

Removes the element at the front of the queue and stores its data in `data`. Returns `0` on success, `-1` if the queue is empty or `data` is `NULL`. A null out-param does not drop the item.

```c
int queue_dequeue(Queue *queue, void **data);

/* Usage */
// queue: 1 2 3
int *t;
queue_dequeue(queue, (void **)&t);
assert(*t == 1);
// queue: 2 3
```

### queue_peek

Returns a `void *` to the front element of the queue without removing it. Returns `NULL` if the queue is empty or the stored payload is `NULL`.

```c
void *queue_peek(Queue *queue);

/* Usage */
// queue: 1 2 3
int *t = (int *)queue_peek(queue);
assert(*t == 1);
// queue: 1 2 3
```

## Macros

### queue_size

Returns the number of elements in the queue.

```c
#define queue_size(q) ((q)->impl.size)
```

### queue_is_empty

Returns true if the queue has no elements.

```c
#define queue_is_empty(q) ((q)->impl.size == 0)
```
