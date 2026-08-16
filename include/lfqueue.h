#ifndef LIBFLINT_QUEUE_H
#define LIBFLINT_QUEUE_H

#include "lflinkedlist.h"

/*
 * Unbounded pointer FIFO over a List. The wrapper type is what
 * keeps list operations from compiling against a Queue.
 */
typedef struct {
    List impl;
} Queue;

#define queue_size(q) ((q)->impl.size)
#define queue_is_empty(q) ((q)->impl.size == 0)

void queue_init(Queue *queue, void (*destroy)(void *data));

void queue_destroy(Queue *queue);

int queue_enqueue(Queue *queue, const void *data);

int queue_dequeue(Queue *queue, void **data);

void *queue_peek(Queue *queue);

#endif
