[libflint](https://git.burkey.co/eburk/libflint) is a library of common C data structures and algorithms, designed to be as agnostic as possible for its users. Over time, it has become my personal library of reusable C code.
## Modules

### Data Structures
- [[linkedlist]] — Doubly-linked list. Used as the foundation for many other modules in libflint
- [[vector]] — Simple type-agnostic dynamic array
- [[stack]] — LIFO (last in first out) stack
- [[queue]] — FIFO (first in first out) queue
- [[set]] — Set built on top of `List`. Ensures uniqueness using a user-provided match function
- [[binarytree]] — Binary tree with standard leaf operations
- [[ringbuf]] — Fixed-size circular buffer for FIFO operations with no runtime allocations after init
- [[bytering]] — Lock-free byte ring buffer for single-producer single-consumer use cases such as UART RX/TX buffers
- [[statemachine]] — Table-driven finite state machine with guards, entry/exit callbacks, and transition actions

### Allocators
- [[memory]] — Arena allocator (bump allocation with save/restore) and pool allocator (fixed-size chunks with free list)

### Utilities
- [[string]] — C string helpers
- [[math]] — General math functions
- [[crypto]] — Cryptographic tools
- [[parsing]] — String parsing tools
- [[input]] — I/O module to assist with consuming data from files
- [[utility]] — Utility code that does not fit anywhere else
- [[bool]] — Macro representation of truthy values
- [[bitops]] — Bit manipulation macros for setting, clearing, toggling, testing, extracting, and inserting bit fields

### Platform
- [[network]] — Generic `Server` type that abstracts away socket setup and teardown
- [[macos]] — Platform-specific code for macOS (conditionally compiled)

## Requirements
### Linux
Requires `libbsd`. This should be available on all major Linux distros.
### macOS, OpenBSD, FreeBSD
Requires no extra dependencies
## Memory Management

The parts of this library that create data structures pass all responsibility of memory management to the user. This is clearly documented in the individual module's documentation.
## Why the name 'libflint'?

`libflint` is named after my dog Flint, who passed away in 2021. I miss you buddy.

![flint](flinty.jpg)