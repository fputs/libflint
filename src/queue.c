#include "lfqueue.h"

void queue_init(Queue *queue, void (*destroy)(void *data)) {
    ll_init(&queue->impl, destroy, NULL);
}

void queue_destroy(Queue *queue) {
    ll_destroy(&queue->impl);
}

int queue_enqueue(Queue *queue, const void *data) {
    return ll_push_tail(&queue->impl, data);
}

int queue_dequeue(Queue *queue, void **data) {
    return ll_pop_head(&queue->impl, data);
}

void *queue_peek(Queue *queue) {
    return queue->impl.head == NULL ? NULL : queue->impl.head->data;
}
