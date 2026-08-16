Lock-free, embedded-friendly byte ring buffer (circular buffer) with no dynamic allocation. Designed for single-producer single-consumer (SPSC) use cases such as UART RX/TX buffers, sensor data streams, and DMA staging. Requires power-of-2 capacity for efficient index wrapping via bitmask. One slot is sacrificed to distinguish full from empty, so a capacity of N holds N-1 bytes.

## Structs

### ByteRing

```c
typedef struct {
    uint8_t *buf;
    size_t capacity;
    size_t mask;
    volatile size_t head;
    volatile size_t tail;
} ByteRing;
```

`head` and `tail` are `volatile` to support ISR/main-loop SPSC usage without locks. The caller owns the backing buffer — `ByteRing` does not allocate or free it.

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

Writes a single byte to the buffer. Returns `0` on success, `-1` if the buffer is full.

```c
int br_write(ByteRing *br, uint8_t byte);

/* Usage */
br_write(&br, 0xAA);
```

### br_read

Reads and removes the oldest byte from the buffer, storing it in `byte`. Returns `0` on success, `-1` if the buffer is empty.

```c
int br_read(ByteRing *br, uint8_t *byte);

/* Usage */
uint8_t byte;
br_read(&br, &byte);
```

### br_peek

Reads the oldest byte without removing it, storing it in `byte`. Returns `0` on success, `-1` if the buffer is empty.

```c
int br_peek(ByteRing *br, uint8_t *byte);

/* Usage */
uint8_t byte;
br_peek(&br, &byte);
```

### br_bulk_write

Writes up to `n` bytes from `src` into the buffer. Returns the number of bytes actually written, which may be less than `n` if the buffer fills up. Returns `0` if `br` or `src` is NULL.

```c
size_t br_bulk_write(ByteRing *br, size_t n, const uint8_t *src);

/* Usage */
uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
size_t written = br_bulk_write(&br, 4, data);
```

### br_bulk_read

Reads up to `n` bytes from the buffer into `dest`. Returns the number of bytes actually read, which may be less than `n` if the buffer doesn't contain enough data. Returns `0` if `br` or `dest` is NULL.

```c
size_t br_bulk_read(ByteRing *br, size_t n, uint8_t *dest);

/* Usage */
uint8_t dest[4];
size_t read = br_bulk_read(&br, 4, dest);
```

### br_reset

Resets the buffer to empty by setting head and tail to 0. Does not zero the backing buffer. The buffer can be reused after reset.

```c
void br_reset(ByteRing *br);
```

## Macros

### br_capacity

Returns the backing-buffer length `N`. Usable storage is `N - 1`.

```c
#define br_capacity(br) ((br)->capacity)
```

### br_available

Returns the number of bytes available to read.

```c
#define br_available(br) (((br)->tail - (br)->head) & (br)->mask)
```

### br_free_space

Returns the number of bytes that can be written. This is `capacity - 1 - available` because one slot is reserved to distinguish full from empty.

```c
#define br_free_space(br) ((br)->capacity - 1 - br_available(br))
```
