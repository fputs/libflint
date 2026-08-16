#ifndef LIBFLINT_BYTE_RING_H
#define LIBFLINT_BYTE_RING_H

#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>

/*
 * Lock-free SPSC byte ring. One producer thread (or ISR) may write;
 * one consumer thread (or main loop) may read. head and tail are
 * atomic: the owner publishes its index with release after touching
 * the buffer, and the other side observes that index with acquire.
 * br_reset is quiescent-only — both sides must be idle.
 */
typedef struct {
    uint8_t *buf;
    size_t capacity;
    size_t mask;
    atomic_size_t head;
    atomic_size_t tail;
} ByteRing;

int br_init(ByteRing *br, uint8_t *buf, size_t capacity);
int br_read(ByteRing *br, uint8_t *byte);
int br_write(ByteRing *br, uint8_t byte);
size_t br_bulk_read(ByteRing *br, size_t n, uint8_t *dest);
size_t br_bulk_write(ByteRing *br, size_t n, const uint8_t *src);
int br_peek(ByteRing *br, uint8_t *byte);
void br_reset(ByteRing *br);

#define br_capacity(br) ((br)->capacity)

/* Observer snapshot: acquire both indices once. */
static inline size_t br_available(const ByteRing *br) {
    size_t tail = atomic_load_explicit(&br->tail, memory_order_acquire);
    size_t head = atomic_load_explicit(&br->head, memory_order_acquire);
    return (tail - head) & br->mask;
}

static inline size_t br_free_space(const ByteRing *br) {
    return br->capacity - 1u - br_available(br);
}

#endif /* LIBFLINT_BYTE_RING_H */
