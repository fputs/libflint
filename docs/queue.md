FIFO queue of `void *`. Use the queue functions to maintain its ordering.

## Usage

Create and initialize a `Queue`. Pass `NULL` for stack allocated payloads, or a destructor such as `free` for payloads the queue should free when destroyed.

```c
Queue queue;
queue_init(&queue, NULL);
int value = 42;
if (queue_enqueue(&queue, &value) == 0) {
    void *data;
    if (queue_dequeue(&queue, &data) == 0) {
        /* data points to value. */
    }
}
queue_destroy(&queue);
```

Removal returns the payload without calling the destructor. Destroy frees the remaining nodes and runs the destructor on their payloads. It does not free the `Queue` handle.

## Structs

### Queue

```c
typedef struct {
    List impl;
} Queue;
```

Enqueue adds at the back. Dequeue and peek read the front.

## Functions

### queue_init

Initializes an empty queue. You own the `Queue` handle. `destroy` is an optional deallocation function for payloads remaining at destruction; pass `NULL` for borrowed data.

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

Removes the element at the front of the queue and stores its data in `data`. Returns `0` on success, `-1` if the queue is empty or `data` is `NULL`. The destructor is not called. A `NULL` output does not remove the item.

```c
int queue_dequeue(Queue *queue, void **data);

/* Usage */
// queue: 1 2 3
void *data;
queue_dequeue(queue, &data);
int *t = data;
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
