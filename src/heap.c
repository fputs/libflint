#include "lfheap.h"
#include "lfcompat.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int lf_heap_init(LfHeap *h, int (*cmp)(const void *, const void *), void (*destroy)(void *)) {
    if (!h) return -1;
    memset(h, 0, sizeof(*h));
    if (!cmp) return -1;
    h->compare = cmp;
    h->destroy = destroy;
    return 0;
}

int lf_heap_reserve(LfHeap *h, size_t capacity) {
    if (!h || !h->compare) return -1;
    if (capacity <= h->cap) return 0;
    void **data = lf_reallocarray(h->data, capacity, sizeof(*data));
    if (!data) return -1;
    h->data = data;
    h->cap = capacity;
    return 0;
}

int lf_heap_push(LfHeap *h, void *value) {
    if (!h || !h->compare) return -1;
    if (h->len == h->cap) {
        size_t max = SIZE_MAX / sizeof(*h->data);
        if (h->cap == max) return -1;
        size_t cap = h->cap ? (h->cap > max / 2 ? max : h->cap * 2) : 8;
        if (lf_heap_reserve(h, cap)) return -1;
    }
    size_t i = h->len++;
    while (i && h->compare(value, h->data[(i - 1) / 2]) < 0) {
        h->data[i] = h->data[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    h->data[i] = value;
    return 0;
}

int lf_heap_peek(const LfHeap *h, void **value) {
    if (!h || !h->compare || !value) return -1;
    if (!h->len) return 1;
    *value = h->data[0];
    return 0;
}

int lf_heap_pop(LfHeap *h, void **value) {
    int status = lf_heap_peek(h, value);
    if (status) return status;
    void *last = h->data[--h->len];
    if (!h->len) return 0;
    size_t i = 0;
    /* i < len/2 guarantees a left child without overflow. */
    while (i < h->len / 2) {
        size_t child = 2 * i + 1;
        if (child + 1 < h->len && h->compare(h->data[child + 1], h->data[child]) < 0)
            child++;
        if (h->compare(last, h->data[child]) <= 0) break;
        h->data[i] = h->data[child];
        i = child;
    }
    h->data[i] = last;
    return 0;
}

void lf_heap_clear(LfHeap *h) {
    if (!h) return;
    if (h->destroy) for (size_t i = 0; i < h->len; i++) h->destroy(h->data[i]);
    h->len = 0;
}

void lf_heap_destroy(LfHeap *h) {
    if (!h) return;
    lf_heap_clear(h);
    free(h->data);
    memset(h, 0, sizeof(*h));
}
