Fixed-size circular buffer (ring buffer) for FIFO operations with no runtime allocations after initialization.

Use `Queue` when the collection must grow without a fixed bound. Use `RingBuffer` when the payload is `void *` and the size is known at init. Use `ByteRing` for a caller-owned byte buffer (UART/ISR-style bytes), not pointers.

## Usage

Include `lfringbuf.h` and initialize a caller-owned `RingBuffer`. Pass `NULL` for the destroy callback when payloads are stack allocated. For heap allocations, pass `free` or your own deallocation function.

```c
RingBuffer rb;
if (rb_init(&rb, 16, NULL) == 0) {
    int value = 1;
    rb_push(&rb, &value);
    void *data;
    if (rb_pop(&rb, &data) == 0) {
        /* data points to value */
    }
    rb_destroy(&rb);
}
```

Destroy the buffer before initializing it again. `rb_destroy` releases its internal storage and any remaining payloads through the callback; the `RingBuffer` handle remains yours.

## Structs

### RingBuffer

```c
typedef struct {
    void **buf;
    size_t capacity;
    size_t count;
    size_t head;
    size_t tail;
    void (*destroy)(void *data);
} RingBuffer;
```

## Functions

### rb_init

Initializes the ring buffer with a fixed capacity. The user is responsible for creating the initial `RingBuffer` structure and freeing memory with `rb_destroy()`. `destroy` is a deallocation function for the members of the buffer, pass `NULL` if the memory is stack-allocated. Returns `0` on success, `-1` on error (NULL pointer, zero capacity, or allocation failure, or allocation size overflow). Failed init zeros the handle so `rb_destroy` is safe.

```c
int rb_init(RingBuffer *rb, size_t capacity, void (*destroy)(void *));

```

### rb_destroy

Calls the deallocation function on any remaining elements in the buffer and frees the internal buffer. Does not destroy the `RingBuffer` struct itself, that is left up to the user.

```c
void rb_destroy(RingBuffer *rb);
```

### rb_push

Adds an element to the back of the buffer. Returns `0` on success, `-1` if the buffer is full, `rb` is `NULL`, or the buffer is not initialized.

```c
int rb_push(RingBuffer *rb, void *data);

/* Usage */
int a = 1;
rb_push(&rb, &a);
```

### rb_force_push

Adds an element to the back. If the buffer is full, the oldest element is dropped first. If a `destroy` callback was given, it is called on the dropped item. Returns `0` on success, `-1` if `rb` is NULL or was not initialized.

```c
int rb_force_push(RingBuffer *rb, void *data);
```

### rb_pop

Removes the element at the front of the buffer and stores its data in `data`. Returns `0` on success, `-1` if the buffer is empty or either argument is `NULL` (the item is left in the buffer). The caller is responsible for freeing the popped element if it was heap-allocated.

```c
int rb_pop(RingBuffer *rb, void **data);

/* Usage */
// buffer: 1 2 3
void *data;
rb_pop(&rb, &data);
assert(*(int *)data == 1);
// buffer: 2 3
```

### rb_peek

Returns a `void *` to the front element of the buffer without removing it. Returns `NULL` if `rb` is `NULL`, the buffer is empty, or the front payload is itself `NULL`.

```c
void *rb_peek(RingBuffer *rb);

/* Usage */
// buffer: 1 2 3
int *t = (int *)rb_peek(&rb);
assert(*t == 1);
// buffer: 1 2 3
```

### rb_clear

Calls the deallocation function on all remaining elements and resets the buffer to empty. The buffer can be reused after clearing.

```c
void rb_clear(RingBuffer *rb);
```

### rb_count

Returns the number of elements currently in the buffer.

```c
#define rb_count(rb) ((rb)->count)
```

### rb_capacity

Returns the total capacity of the buffer.

```c
#define rb_capacity(rb) ((rb)->capacity)
```

### rb_is_empty

Returns true if the buffer has no elements.

```c
#define rb_is_empty(rb) ((rb)->count == 0)
```

### rb_is_full

Returns true if the buffer is at capacity.

```c
#define rb_is_full(rb) ((rb)->count == (rb)->capacity)
```
