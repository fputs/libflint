A byte ring for one producer and one consumer, with no dynamic allocation. One producer and one consumer may run concurrently. The implementation uses C11 atomics; whether those atomics are lock-free depends on the target. Capacity must be a power of two. One slot is reserved, so a capacity of N holds N-1 bytes.

You provide the backing buffer. `ByteRing` does not allocate or free it. Use `br_write` / `br_bulk_write` from the producer only, and `br_read` / `br_bulk_read` from the consumer only. Call `br_reset` only when both sides are idle.

## Usage

Include `lfbytering.h`. Provide the backing storage and initialize a caller-owned `ByteRing`. The storage must outlive the ring. No destroy function is needed.

```c
uint8_t storage[64];
ByteRing ring;
if (br_init(&ring, storage, sizeof(storage)) == 0) {
    br_write(&ring, 0xAA);
    uint8_t byte;
    if (br_read(&ring, &byte) == 0) {
        /* byte == 0xAA */
    }
}
```

Use the ring only after successful initialization. Bulk source and destination buffers must not overlap the backing storage.

## Structs

### ByteRing

```c
typedef struct {
    uint8_t *buf;
    size_t capacity;
    size_t mask;
    atomic_size_t head;
    atomic_size_t tail;
} ByteRing;
```

Do not load or store `head` / `tail` directly. Use the functions and `br_available` / `br_free_space`.

## Functions

### br_init

Initializes the byte ring buffer with a caller-provided backing buffer. `capacity` must be a power of 2 and at least 2. One slot is reserved, so usable size is `N - 1`. Returns `0` on success, `-1` on error (NULL pointer, `capacity < 2`, or non-power-of-2 capacity).

```c
int br_init(ByteRing *br, uint8_t *buf, size_t capacity);

/* Usage */
uint8_t buf[64];
ByteRing br;
br_init(&br, buf, 64);
```

### br_write

Writes a single byte to the buffer. Producer-only. Returns `0` on success, `-1` if the buffer is full or `br` is `NULL`.

```c
int br_write(ByteRing *br, uint8_t byte);

/* Usage */
br_write(&br, 0xAA);
```

### br_read

Reads and removes the oldest byte from the buffer, storing it in `byte`. Consumer-only. Returns `0` on success, `-1` if the buffer is empty or either pointer is `NULL`.

```c
int br_read(ByteRing *br, uint8_t *byte);

/* Usage */
uint8_t byte;
br_read(&br, &byte);
```

### br_peek

Reads the oldest byte without removing it, storing it in `byte`. Consumer-only. Returns `0` on success, `-1` if the buffer is empty or either pointer is `NULL`.

```c
int br_peek(ByteRing *br, uint8_t *byte);

/* Usage */
uint8_t byte;
br_peek(&br, &byte);
```

### br_bulk_write

Writes up to `n` bytes from `src` into the buffer. Producer-only. Returns the number of bytes actually written, which may be less than `n` if the buffer fills up. Returns `0` if `br` or `src` is NULL.

```c
size_t br_bulk_write(ByteRing *br, size_t n, const uint8_t *src);

/* Usage */
uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
size_t written = br_bulk_write(&br, 4, data);
```

### br_bulk_read

Reads up to `n` bytes from the buffer into `dest`. Consumer-only. Returns the number of bytes actually read, which may be less than `n` if the buffer doesn't contain enough data. Returns `0` if `br` or `dest` is NULL.

```c
size_t br_bulk_read(ByteRing *br, size_t n, uint8_t *dest);

/* Usage */
uint8_t dest[4];
size_t read = br_bulk_read(&br, 4, dest);
```

### br_reset

Resets the buffer to empty by setting head and tail to 0. Does not zero the backing buffer. The buffer can be reused after reset. Both producer and consumer must be idle; concurrent reset is not lock-free.

```c
void br_reset(ByteRing *br);
```

### br_capacity

Returns the backing-buffer length `N`. Usable storage is `N - 1`.

```c
#define br_capacity(br) ((br)->capacity)
```

### br_available

Returns the number of bytes available to read. Loads both indices with acquire ordering. With concurrent activity this is an observation, not a reservation of bytes. Requires a non-NULL initialized ring.

```c
static inline size_t br_available(const ByteRing *br);
```

### br_free_space

Returns the number of bytes that can be written. This is `capacity - 1 - available` because one slot is reserved to distinguish full from empty.

```c
static inline size_t br_free_space(const ByteRing *br);
```
