#ifndef LIBFLINT_QUEUE_H
#define LIBFLINT_QUEUE_H

#include "lflinkedlist.h"

#define Queue List

void queue_init(Queue *queue, void (*destroy)(void *data));

void queue_destroy(Queue *queue);

int queue_enqueue(Queue *queue, void *data);

int queue_dequeue(Queue *queue, void **data);

void *queue_peek(Queue *queue);

#endif
