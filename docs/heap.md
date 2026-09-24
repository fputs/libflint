A priority queue of `void *` values. The comparator determines which payload is removed first.

## Usage

Initialize a `LfHeap` with a comparator and an optional payload destructor. The comparator receives payload pointers directly. A negative result places its first argument before its second. Reverse the comparison for a maximum-first queue.

```c
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

The example uses stack allocated data, so it passes `NULL` for the destructor. `lf_heap_pop` returns the payload without destroying it. Destroy frees the array and calls the destructor on remaining payloads; the `LfHeap` handle is yours to free if you allocated it.

Equal priorities have unspecified order. Payloads must not change priority while stored, and callbacks must not mutate the heap. `NULL` payloads are allowed if the comparator accepts them. Do not share owned allocations between entries. Concurrent mutation of the heap is not supported.

## Structs

### LfHeap

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

Initializes an empty heap without allocating. Returns `0` on success, `-1` if the handle or comparator is `NULL`. Failed initialization leaves a non-`NULL` handle destroyable. Do not initialize a live heap.

```c
int lf_heap_init(LfHeap *heap, int (*compare)(const void *, const void *),
                 void (*destroy)(void *));
```

### lf_heap_reserve

Ensures total capacity for at least `capacity` entries. Returns `0` on success,
`-1` on invalid arguments, overflow, or allocation failure. Failure leaves the heap unchanged. Reserve may relocate the pointer array.

```c
int lf_heap_reserve(LfHeap *heap, size_t capacity);
```

### lf_heap_push

Inserts a payload in priority order. Returns `0` on success, `-1` on allocation or argument failure. Failure leaves contents and ownership unchanged. Push takes O(log n) amortized time; growing the array may copy O(n) pointers.

```c
int lf_heap_push(LfHeap *heap, void *value);
```

### lf_heap_peek / lf_heap_pop

Returns `0` on success, `1` if empty, `-1` on invalid arguments. Peek borrows the
pointer; pop transfers it without calling the destructor. The required output is unchanged on failure and must not alias heap storage. Peek takes O(1) time; pop takes O(log n).

```c
int lf_heap_peek(const LfHeap *heap, void **value);
int lf_heap_pop(LfHeap *heap, void **value);
```

### lf_heap_clear / lf_heap_destroy

Clear destroys entries while retaining capacity. Destroy also frees the array
and zeros the handle. The destructor is called for each entry, including `NULL`. Clear takes O(n) time. Both functions accept a `NULL` handle.

```c
void lf_heap_clear(LfHeap *heap);
void lf_heap_destroy(LfHeap *heap);
```

## Macros

```c
#define lf_heap_size(h) ((h)->len)
```
