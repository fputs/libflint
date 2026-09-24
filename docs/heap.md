# lfheap

Include `lfheap.h` and initialize a caller-owned `LfHeap` with a comparator and an
optional destructor. The comparator receives payload pointers directly; a negative
result places its first argument before its second. Reverse the comparison to
obtain a maximum-first queue. Equal priorities have unspecified order. Payloads
must not change priority while stored, and callbacks must not mutate the heap.

`lf_heap_reserve` reserves total entry capacity, not additional space. Push takes
ownership on success (0); allocation or argument failure returns -1 without
changing contents. Capacity growth checks arithmetic and uses a contiguous pointer
array, with no per-item allocation. Init does not allocate; failed init leaves a
destroyable handle. Never initialize a live heap.

`lf_heap_peek(heap, &value)` borrows the first pointer; `lf_heap_pop` transfers it.
Both return 0 on success, 1 if empty, -1 for invalid arguments and leave the output
unchanged on failure. Outputs must not alias heap storage. NULL payloads work if
the comparator accepts them. Clear retains capacity and invokes the destructor
for each entry, including NULL; destroy additionally frees storage and zeros the
handle. NULL destructor means borrowed payloads. Owned allocations must not be
shared between entries. Clear and destroy accept NULL handles.

Push and pop take O(log n), peek O(1), clear O(n). Reserve may relocate storage.
There is no synchronization.

## Structs

```c
typedef struct {
    void **data;
    size_t len, cap;
    int (*compare)(const void *, const void *);
    void (*destroy)(void *);
} LfHeap;
```

## Functions

### lf_heap_init

Returns 0 on success, -1 if the handle or comparator is NULL.

```c
int lf_heap_init(LfHeap *heap, int (*compare)(const void *, const void *),
                 void (*destroy)(void *));

/* Usage: minimum-first queue of borrowed integers */
int int_compare(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

LfHeap heap;
if (lf_heap_init(&heap, int_compare, NULL) == 0) {
    int priority = 5;
    if (lf_heap_push(&heap, &priority) == 0) {
        void *item;
        if (lf_heap_pop(&heap, &item) == 0) {
            /* item == &priority */
        }
    }
    lf_heap_destroy(&heap);
}
```

### lf_heap_reserve

Ensures total capacity for at least `capacity` entries. Returns 0 on success,
-1 on invalid arguments, overflow, or allocation failure.

```c
int lf_heap_reserve(LfHeap *heap, size_t capacity);
```

### lf_heap_push

Returns 0 on success, -1 on error. Ownership passes only on success.

```c
int lf_heap_push(LfHeap *heap, void *value);
```

### lf_heap_peek / lf_heap_pop

Returns 0 on success, 1 if empty, -1 on invalid arguments. Peek borrows the
pointer; pop transfers it. The required output is unchanged on failure.

```c
int lf_heap_peek(const LfHeap *heap, void **value);
int lf_heap_pop(LfHeap *heap, void **value);
```

### lf_heap_clear / lf_heap_destroy

Clear destroys entries while retaining capacity. Destroy also frees the array
and zeros the handle.

```c
void lf_heap_clear(LfHeap *heap);
void lf_heap_destroy(LfHeap *heap);
```

## Macros

```c
#define lf_heap_size(h) ((h)->len)
```
