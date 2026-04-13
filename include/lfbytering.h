#ifndef LIBFLINT_BYTE_RING_H
#define LIBFLINT_BYTE_RING_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t *buf;
    size_t capacity;
    size_t mask;
    volatile size_t head;
    volatile size_t tail;
} ByteRing;

int br_init(ByteRing *br, uint8_t *buf, size_t capacity);
int br_read(ByteRing *br, uint8_t *byte);
int br_write(ByteRing *br, uint8_t byte);
size_t br_bulk_read(ByteRing *br, size_t n, uint8_t *dest);
size_t br_bulk_write(ByteRing *br, size_t n, const uint8_t *src);
int br_peek(ByteRing *br, uint8_t *byte);
void br_reset(ByteRing *br);

#define br_capacity(br) ((br)->capacity)
#define br_available(br) (((br)->tail - (br)->head) & (br)->mask)
#define br_free_space(br) ((br)->capacity - 1 - br_available(br))

#endif //LIBFLINT_BYTE_RING_H
