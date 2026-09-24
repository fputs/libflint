#ifndef LIBFLINT_H_BYTEBUF
#define LIBFLINT_H_BYTEBUF
#include <stddef.h>
#include <stdint.h>

typedef enum { LF_BYTEBUF_LE, LF_BYTEBUF_BE } LfByteOrder;
typedef struct {
    const unsigned char *data;
    unsigned char *writable;
    size_t len, pos;
} LfByteBuf;

/* Borrow storage. NULL is valid only for a zero-length buffer. */
int lf_bytebuf_init(LfByteBuf *buf, void *data, size_t length);
int lf_bytebuf_init_reader(LfByteBuf *buf, const void *data, size_t length);
size_t lf_bytebuf_remaining(const LfByteBuf *buf);
/* 0 success, -1 error; failed operations do not advance or modify output. */
int lf_bytebuf_read(LfByteBuf *buf, void *dest, size_t length);
int lf_bytebuf_write(LfByteBuf *buf, const void *src, size_t length);
int lf_bytebuf_skip(LfByteBuf *buf, size_t length);
int lf_bytebuf_read_u8(LfByteBuf *buf, uint8_t *value);
int lf_bytebuf_write_u8(LfByteBuf *buf, uint8_t value);
int lf_bytebuf_read_u16(LfByteBuf *buf, LfByteOrder order, uint16_t *value);
int lf_bytebuf_write_u16(LfByteBuf *buf, LfByteOrder order, uint16_t value);
int lf_bytebuf_read_u32(LfByteBuf *buf, LfByteOrder order, uint32_t *value);
int lf_bytebuf_write_u32(LfByteBuf *buf, LfByteOrder order, uint32_t value);
int lf_bytebuf_read_u64(LfByteBuf *buf, LfByteOrder order, uint64_t *value);
int lf_bytebuf_write_u64(LfByteBuf *buf, LfByteOrder order, uint64_t value);
#endif
