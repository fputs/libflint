#ifndef LIBFLINT_RING_BUF_H
#define LIBFLINT_RING_BUF_H

#include <stddef.h>

typedef struct {
    void **buf;
    size_t capacity;
    size_t count;
    size_t head;
    size_t tail;
    void (*destroy)(void *data);
} RingBuffer;

int rb_init(RingBuffer *rb, size_t capacity, void (*destroy)(void *));
void rb_destroy(RingBuffer *rb);
int rb_push(RingBuffer *rb, void *data);
int rb_force_push(RingBuffer *rb, void *data);
int rb_pop(RingBuffer *rb, void **data);
void *rb_peek(RingBuffer *rb);
void rb_clear(RingBuffer *rb);

#define rb_count(rb) ((rb)->count)
#define rb_capacity(rb) ((rb)->capacity)
#define rb_is_empty(rb) ((rb)->count == 0)
#define rb_is_full(rb) ((rb)->count == (rb)->capacity)

#endif //LIBFLINT_RING_BUF_H
