#ifndef LIBFLINT_H_HEAP
#define LIBFLINT_H_HEAP
#include <stddef.h>

typedef struct {
    void **data;
    size_t len, cap;
    int (*compare)(const void *, const void *);
    void (*destroy)(void *);
} LfHeap;

#define lf_heap_size(h) ((h)->len)
/* Negative comparison means a comes before b. Init does not allocate. */
int lf_heap_init(LfHeap *heap, int (*compare)(const void *, const void *),
                 void (*destroy)(void *));
/* Reserve total entry capacity; 0 success, -1 error, unchanged on failure. */
int lf_heap_reserve(LfHeap *heap, size_t capacity);
int lf_heap_push(LfHeap *heap, void *value);
/* 0 success, 1 empty, -1 invalid arguments; output unchanged on failure.
 * Peek borrows the pointer; pop transfers ownership, including NULL values. */
int lf_heap_peek(const LfHeap *heap, void **value);
int lf_heap_pop(LfHeap *heap, void **value);
void lf_heap_clear(LfHeap *heap);
void lf_heap_destroy(LfHeap *heap);
#endif
