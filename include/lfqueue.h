#ifndef LIBFLINT_QUEUE_H
#define LIBFLINT_QUEUE_H

#include "lflinkedlist.h"

#define Queue List
#define queue_size(q) ((q)->size)
#define queue_is_empty(q) ((q)->size == 0)

void queue_init(Queue *queue, void (*destroy)(void *data));

void queue_destroy(Queue *queue);

int queue_enqueue(Queue *queue, const void *data);

int queue_dequeue(Queue *queue, void **data);

void *queue_peek(Queue *queue);

#endif
