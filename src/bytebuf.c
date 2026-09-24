#include "lfbytebuf.h"
#include <limits.h>
#include <string.h>

#if CHAR_BIT != 8
#error "lfbytebuf requires 8-bit bytes"
#endif

int lf_bytebuf_init_reader(LfByteBuf *b, const void *data, size_t len) {
    if (!b) return -1;
    memset(b, 0, sizeof(*b));
    if (!data && len) return -1;
    b->data = data;
    b->len = len;
    return 0;
}

int lf_bytebuf_init(LfByteBuf *b, void *data, size_t len) {
    if (lf_bytebuf_init_reader(b, data, len)) return -1;
    b->writable = data;
    return 0;
}

size_t lf_bytebuf_remaining(const LfByteBuf *b) {
    return b && b->pos <= b->len ? b->len - b->pos : 0;
}

static int fits(const LfByteBuf *b, size_t n) {
    return b && b->pos <= b->len && n <= b->len - b->pos;
}

int lf_bytebuf_skip(LfByteBuf *b, size_t n) {
    if (!fits(b, n)) return -1;
    b->pos += n;
    return 0;
}

int lf_bytebuf_read(LfByteBuf *b, void *dest, size_t n) {
    if (!fits(b, n) || (n && !dest)) return -1;
    if (n) memmove(dest, b->data + b->pos, n);
    b->pos += n;
    return 0;
}

int lf_bytebuf_write(LfByteBuf *b, const void *src, size_t n) {
    if (!fits(b, n) || (n && (!src || !b->writable))) return -1;
    if (n) memmove(b->writable + b->pos, src, n);
    b->pos += n;
    return 0;
}

static int read_integer(LfByteBuf *b, LfByteOrder order, size_t n, uint64_t *value) {
    if (!fits(b, n) || (order != LF_BYTEBUF_LE && order != LF_BYTEBUF_BE)) return -1;
    uint64_t result = 0;
    for (size_t i = 0; i < n; i++) {
        size_t shift = (order == LF_BYTEBUF_LE ? i : n - 1 - i) * 8;
        result |= (uint64_t)b->data[b->pos + i] << shift;
    }
    b->pos += n;
    *value = result;
    return 0;
}

static int write_integer(LfByteBuf *b, LfByteOrder order, size_t n, uint64_t value) {
    if (!fits(b, n) || !b->writable || (order != LF_BYTEBUF_LE && order != LF_BYTEBUF_BE)) return -1;
    for (size_t i = 0; i < n; i++) {
        size_t shift = (order == LF_BYTEBUF_LE ? i : n - 1 - i) * 8;
        b->writable[b->pos + i] = (unsigned char)(value >> shift);
    }
    b->pos += n;
    return 0;
}

int lf_bytebuf_read_u8(LfByteBuf *b, uint8_t *value) {
    if (!value) return -1;
    uint64_t v;
    if (read_integer(b, LF_BYTEBUF_LE, 1, &v)) return -1;
    *value = (uint8_t)v;
    return 0;
}

int lf_bytebuf_write_u8(LfByteBuf *b, uint8_t value) {
    return write_integer(b, LF_BYTEBUF_LE, 1, value);
}

#define INTEGER_API(bits) \
int lf_bytebuf_read_u##bits(LfByteBuf *b, LfByteOrder order, uint##bits##_t *value) { \
    if (!value) return -1; \
    uint64_t v; \
    if (read_integer(b, order, bits / 8, &v)) return -1; \
    *value = (uint##bits##_t)v; \
    return 0; \
} \
int lf_bytebuf_write_u##bits(LfByteBuf *b, LfByteOrder order, uint##bits##_t value) { \
    return write_integer(b, order, bits / 8, value); \
}

INTEGER_API(16)
INTEGER_API(32)
INTEGER_API(64)
