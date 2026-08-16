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
    br->head = 0;
    br->tail = 0;

    return 0;
}

int br_read(ByteRing *br, uint8_t *byte) {
    if (br == NULL || byte == NULL || br_available(br) == 0) {
        return -1;
    }

    *byte = br->buf[br->head];
    br->head = (br->head + 1) & br->mask;
    return 0;
}

int br_write(ByteRing *br, uint8_t byte) {
    if (br == NULL || br_free_space(br) == 0) {
        return -1;
    }

    br->buf[br->tail] = byte;
    br->tail = (br->tail + 1) & br->mask;
    return 0;
}

size_t br_bulk_read(ByteRing *br, const size_t n, uint8_t *dest) {
    if (br == NULL || dest == NULL) {
        return 0;
    }
    size_t a = br_available(br);
    const size_t available = a > n ? n : a;
    const size_t to_end = br->capacity - br->head;
    size_t chunk_sz = to_end > available ? available : to_end;
    memcpy(dest, br->buf + br->head, chunk_sz);

    /* needs to wrap */
    if (chunk_sz < available) {
        memcpy(dest + chunk_sz, br->buf, available - chunk_sz);
    }
    br->head = (br->head + available) & br->mask;
    return available;
}

size_t br_bulk_write(ByteRing *br, const size_t n, const uint8_t *src) {
    if (br == NULL || src == NULL) {
        return 0;
    }
    size_t a = br_free_space(br);
    const size_t fs = a > n ? n : a;
    const size_t to_end = br->capacity - br->tail;
    size_t chunk_sz = to_end > fs ? fs : to_end;
    memcpy(br->buf + br->tail, src, chunk_sz);

    /* needs to wrap */
    if (chunk_sz < fs) {
        memcpy(br->buf, src + chunk_sz, fs - chunk_sz);
    }
    br->tail = (br->tail + fs) & br->mask;
    return fs;
}

int br_peek(ByteRing *br, uint8_t *byte) {
    if (br == NULL || byte == NULL || br_available(br) == 0) {
        return -1;
    }
    *byte = br->buf[br->head];
    return 0;
}

void br_reset(ByteRing *br) {
    br->head = 0;
    br->tail = 0;
}
