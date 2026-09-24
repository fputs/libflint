[libflint](https://git.burkey.co/eburk/libflint) is a C library of containers and helpers. Handles are caller-owned; payloads are `void *`. Each module page covers ownership, return codes, and examples.

## Modules

### Data Structures
- [[linkedlist]]: Doubly-linked list of `void *`. Foundation for stack, queue, and set
- [[vector]]: Growable array of `void *`
- [[stack]]: LIFO stack of `void *`
- [[heap]]: Comparator-ordered pointer priority queue
- [[queue]]: Unbounded FIFO queue
- [[set]]: Small unique collection. Membership is linear
- [[hashmap]]: Generic key/value map with independent key and value ownership
- [[hashset]]: Unique collection with hashed membership
- [[binarytree]]: Binary tree you build by attaching left and right children
- [[bst]]: Ordered binary search tree (`insert`, `find`, `remove`)
- [[ringbuf]]: Fixed-capacity FIFO of `void *`. No allocation after init
- [[bytering]]: Lock-free byte ring for one producer and one consumer
- [[statemachine]]: Table-driven finite state machine with guards and entry/exit callbacks

### Allocators
- [[memory]]: Arena (bump allocation with save/restore) and pool (fixed-size chunks)

### Utilities
- [[strbuf]]: Owned growable string builder
- [[string]]: `lfstr_*` helpers and `LfStr` views
- [[math]]: Integer helpers, `LfPoint`, and Bresenham
- [[codec]]: Hex and Base64 (encoding, not cryptography)
- [[puzzle]]: Repeating-key XOR, Hamming distance, English scoring
- [[crypto]]: Includes [[codec]] and [[puzzle]]
- [[input]]: Read a regular, seekable file into memory
- [[process]]: `lf_capture_system` (`popen`; the command is not sanitized)
- [[bitset]]: Runtime-sized compact bit array
- [[bitops]]: Macros to set, clear, toggle, test, extract, and insert bit fields

### Platform
- [[network]]: IPv4 `lf_listen_tcp`, `lf_bind_udp`, and `lf_close`
- [[macos]]: Process CPU and memory sampling (Apple only)
- [[compat]]: Overflow-checked `lf_reallocarray` on every platform

## Requirements
A C99 compiler and CMake. Linux needs `libbsd` (`strtonum` in the input module). macOS, OpenBSD, and FreeBSD need no extra libraries.
## Memory Management

You allocate the handle (`List`, `Vector`, and so on). The library allocates internal nodes and buffers. A `destroy` callback, if you pass one, runs when the container is destroyed or cleared, not when you pop or remove an item.
## Why the name 'libflint'?

`libflint` is named after my dog Flint, who passed away in 2021. I miss you buddy.

![flint](flinty.jpg)