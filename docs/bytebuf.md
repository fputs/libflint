A bounded binary reader/writer over caller-provided storage. Include
`lfbytebuf.h`. Complements [[codec]] and [[network]]. No allocation or ownership
transfer occurs; the storage must outlive the cursor. Requires 8-bit bytes.

## Structs

```c
typedef enum { LF_BYTEBUF_LE, LF_BYTEBUF_BE } LfByteOrder;
typedef struct {
    const unsigned char *data;
    unsigned char *writable;
    size_t len, pos;
} LfByteBuf;
```

`len` is the accessible byte count; `pos` is the next byte offset. Reader-only
handles have NULL `writable`. Treat fields as read-only and reinitialize to rewind.
Each operation checks bounds before touching storage and leaves cursor, storage,
and output unchanged on failure. Output pointers must not alias handle fields;
integer outputs must not alias backing storage. Raw byte copies support overlap.

## Functions

### lf_bytebuf_init / lf_bytebuf_init_reader

Return 0 on success, -1 for invalid arguments. NULL storage is allowed only with
zero length. Failed initialization zeros a non-NULL handle. Init allows reads and
writes; init_reader accepts const storage and rejects nonempty writes. The caller
is responsible for providing the stated amount of accessible storage. No destroy
function is needed.

```c
int lf_bytebuf_init(LfByteBuf *buf, void *data, size_t length);
int lf_bytebuf_init_reader(LfByteBuf *buf, const void *data, size_t length);

/* Usage */
unsigned char packet[4];
LfByteBuf cursor;
lf_bytebuf_init(&cursor, packet, sizeof(packet));
if (lf_bytebuf_write_u32(&cursor, LF_BYTEBUF_BE, UINT32_C(0x12345678)) == 0) {
    /* packet contains 12 34 56 78 in hexadecimal */
}
lf_bytebuf_init_reader(&cursor, packet, sizeof(packet));
uint32_t value;
lf_bytebuf_read_u32(&cursor, LF_BYTEBUF_BE, &value);
```

### lf_bytebuf_remaining

Return accessible bytes after the cursor; NULL returns zero.

```c
size_t lf_bytebuf_remaining(const LfByteBuf *buf);
```

### lf_bytebuf_read / lf_bytebuf_write / lf_bytebuf_skip

Copy or skip bytes, advancing only on success. Return 0 on success, -1 for invalid
arguments, insufficient capacity, or a nonempty write to a reader. Zero-length
operations are valid with NULL source/destination pointers, including on an empty
buffer. These operations neither interpret nor terminate strings.

```c
int lf_bytebuf_read(LfByteBuf *buf, void *dest, size_t length);
int lf_bytebuf_write(LfByteBuf *buf, const void *src, size_t length);
int lf_bytebuf_skip(LfByteBuf *buf, size_t length);
```

### Fixed-width integers

Read or write unsigned 8-, 16-, 32-, or 64-bit integers. Multi-byte operations
require explicit little-endian (`LF_BYTEBUF_LE`) or big-endian (`LF_BYTEBUF_BE`)
order. Invalid orders return -1. Return 0 on success, -1 on failure. Read outputs
are required. Bytewise operations avoid unaligned integer loads and stores and
are independent of host byte order.

```c
int lf_bytebuf_read_u8(LfByteBuf *buf, uint8_t *value);
int lf_bytebuf_write_u8(LfByteBuf *buf, uint8_t value);
int lf_bytebuf_read_u16(LfByteBuf *buf, LfByteOrder order, uint16_t *value);
int lf_bytebuf_write_u16(LfByteBuf *buf, LfByteOrder order, uint16_t value);
int lf_bytebuf_read_u32(LfByteBuf *buf, LfByteOrder order, uint32_t *value);
int lf_bytebuf_write_u32(LfByteBuf *buf, LfByteOrder order, uint32_t value);
int lf_bytebuf_read_u64(LfByteBuf *buf, LfByteOrder order, uint64_t *value);
int lf_bytebuf_write_u64(LfByteBuf *buf, LfByteOrder order, uint64_t value);
```
