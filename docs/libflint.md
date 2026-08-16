[libflint](https://git.burkey.co/eburk/libflint) is a library of common C data structures and algorithms, designed to be as agnostic as possible for its users. Over time, it has become my personal library of reusable C code.
## Modules

### Data Structures
- [[linkedlist]] — Doubly-linked list. Used as the foundation for many other modules in libflint
- [[vector]] — Simple type-agnostic dynamic array
- [[stack]] — LIFO stack. `Stack` wraps a `List`; top is the tail
- [[queue]] — FIFO queue. `Queue` wraps a `List`; dequeue is at the head
- [[set]] — Small uniqueness-constrained set over a `List` wrapper. Membership is O(n)
- [[hashset]] — Open-addressed `void *` set
- [[binarytree]] — Binary tree node / forest kit (`ins_left`, `merge`, prune subtree)
- [[bst]] — Ordered BST on top of the binary-tree kit
- [[ringbuf]] — Fixed-size circular buffer for FIFO operations with no runtime allocations after init
- [[bytering]] — Lock-free byte ring buffer for single-producer single-consumer use cases such as UART RX/TX buffers
- [[statemachine]] — Table-driven finite state machine with guards, entry/exit callbacks, and transition actions

### Allocators
- [[memory]] — Arena allocator (bump allocation with save/restore) and pool allocator (fixed-size chunks with free list)

### Utilities
- [[string]] — `lfstr_*` helpers and `LfStr` views
- [[math]] — Integer helpers, `LfPoint`, and Bresenham
- [[codec]] — Hex and Base64 codecs (not cryptography)
- [[puzzle]] — Repeating-key XOR, Hamming distance, English scoring (Cryptopals-style, not cryptography)
- [[crypto]] — Compatibility include of codec + puzzle
- [[input]] — One-shot slurp of a regular, seekable file; lines via `lfstr_split`
- [[process]] — `lf_capture_system` (`popen`, unsanitized)
- [[bitops]] — Bit manipulation macros for setting, clearing, toggling, testing, extracting, and inserting bit fields

### Platform
- [[network]] — IPv4 `lf_listen_tcp` / `lf_bind_udp` / `lf_close` (no Server runloop)
- [[macos]] — Platform-specific code for macOS (conditionally compiled)
- [[compat]] — Overflow-checked `lf_reallocarray` for every platform

## Requirements
A C99 compiler and CMake. Linux needs `libbsd` (`strtonum` in the input module). macOS, OpenBSD, and FreeBSD need no extra libraries.
## Memory Management

The parts of this library that create data structures pass all responsibility of memory management to the user. This is clearly documented in the individual module's documentation.
## Why the name 'libflint'?

`libflint` is named after my dog Flint, who passed away in 2021. I miss you buddy.

![flint](flinty.jpg)