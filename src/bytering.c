#include <string.h>

#include "lfbytering.h"

int br_init(ByteRing *br, uint8_t *buf, const size_t capacity) {
    if (br == NULL || buf == NULL) {
        return -1;
    }

    if (capacity < 2 || (capacity & (capacity - 1)) != 0) {
        return -1;
    }

    br->buf = buf;
    br->capacity = capacity;
    br->mask = capacity - 1;
    atomic_store_explicit(&br->head, 0, memory_order_relaxed);
    atomic_store_explicit(&br->tail, 0, memory_order_relaxed);

    return 0;
}

int br_read(ByteRing *br, uint8_t *byte) {
    if (br == NULL || byte == NULL) {
        return -1;
    }

    size_t head = atomic_load_explicit(&br->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&br->tail, memory_order_acquire);
    if (((tail - head) & br->mask) == 0) {
        return -1;
    }

    *byte = br->buf[head];
    atomic_store_explicit(&br->head, (head + 1) & br->mask, memory_order_release);
    return 0;
}

int br_write(ByteRing *br, uint8_t byte) {
    if (br == NULL) {
        return -1;
    }

    size_t tail = atomic_load_explicit(&br->tail, memory_order_relaxed);
    size_t head = atomic_load_explicit(&br->head, memory_order_acquire);
    if (br->capacity - 1u - ((tail - head) & br->mask) == 0) {
        return -1;
    }

    br->buf[tail] = byte;
    atomic_store_explicit(&br->tail, (tail + 1) & br->mask, memory_order_release);
    return 0;
}

size_t br_bulk_read(ByteRing *br, const size_t n, uint8_t *dest) {
    if (br == NULL || dest == NULL) {
        return 0;
    }

    size_t head = atomic_load_explicit(&br->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&br->tail, memory_order_acquire);
    size_t a = (tail - head) & br->mask;
    const size_t available = a > n ? n : a;
    const size_t to_end = br->capacity - head;
    size_t chunk_sz = to_end > available ? available : to_end;
    memcpy(dest, br->buf + head, chunk_sz);

    if (chunk_sz < available) {
        memcpy(dest + chunk_sz, br->buf, available - chunk_sz);
    }
    atomic_store_explicit(&br->head, (head + available) & br->mask,
                          memory_order_release);
    return available;
}

size_t br_bulk_write(ByteRing *br, const size_t n, const uint8_t *src) {
    if (br == NULL || src == NULL) {
        return 0;
    }

    size_t tail = atomic_load_explicit(&br->tail, memory_order_relaxed);
    size_t head = atomic_load_explicit(&br->head, memory_order_acquire);
    size_t a = br->capacity - 1u - ((tail - head) & br->mask);
    const size_t fs = a > n ? n : a;
    const size_t to_end = br->capacity - tail;
    size_t chunk_sz = to_end > fs ? fs : to_end;
    memcpy(br->buf + tail, src, chunk_sz);

    if (chunk_sz < fs) {
        memcpy(br->buf, src + chunk_sz, fs - chunk_sz);
    }
    atomic_store_explicit(&br->tail, (tail + fs) & br->mask,
                          memory_order_release);
    return fs;
}

int br_peek(ByteRing *br, uint8_t *byte) {
    if (br == NULL || byte == NULL) {
        return -1;
    }

    size_t head = atomic_load_explicit(&br->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&br->tail, memory_order_acquire);
    if (((tail - head) & br->mask) == 0) {
        return -1;
    }
    *byte = br->buf[head];
    return 0;
}

void br_reset(ByteRing *br) {
    if (br == NULL) {
        return;
    }
    /* Quiescent only: both producer and consumer must be idle. */
    atomic_store_explicit(&br->head, 0, memory_order_relaxed);
    atomic_store_explicit(&br->tail, 0, memory_order_relaxed);
}
