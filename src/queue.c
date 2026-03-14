#include "lfqueue.h"

void queue_init(Queue *queue, void (*destroy)(void *data)) {
    ll_init(queue, destroy);
}

void queue_destroy(Queue *queue) {
    ll_destroy(queue);
}

int queue_enqueue(Queue *queue, void *data) {
    if (queue->size == 0) {
        return ll_ins_next(queue, NULL, data);
    }
    return ll_ins_next(queue, queue->tail, data);
}

int queue_dequeue(Queue *queue, void **data) {
    return ll_remove(queue, queue->head, data);
}

void *queue_peek(Queue *queue) {
    return queue->head == NULL ? NULL : queue->head->data;
}
