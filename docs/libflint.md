[libflint](https://github.com/fputs/libflint) is a C library of containers and helpers for Linux, macOS, OpenBSD, and FreeBSD. Container handles are generally caller-owned; pointer containers store `void *` payloads. Each module page covers ownership, return codes, and examples.

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
- [[bst]]: Unbalanced ordered binary search tree (`insert`, `find`, `remove`)
- [[ringbuf]]: Fixed-capacity FIFO of `void *`. No allocation after init
- [[bytering]]: Byte ring with caller-provided storage and atomic indices for one producer and one consumer. Lock-free operation depends on the target
- [[statemachine]]: Table-driven finite state machine with guards and entry/exit callbacks

### Allocators

- [[memory]]: Arena (bump allocation with save/restore) and pool (fixed-size chunks)

### Utilities

- [[strbuf]]: Owned growable string builder
- [[parse]]: Integer and token parsing from borrowed string views
- [[string]]: `lfstr_*` helpers and `LfStr` views
- [[math]]: Integer helpers, `LfPoint`, and Bresenham
- [[bytebuf]]: Bounded binary cursor with explicit byte order
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

CMake 3.17 or newer and a C compiler. The build selects C99, but [[bytering]] also requires support for C11 atomics (`<stdatomic.h>` and `atomic_size_t`). The compiler must support those in the selected language mode.

Linux needs the `libbsd` headers and library (`strtonum` in the input module). macOS, OpenBSD, and FreeBSD need no extra libraries.

## Building and Testing

From the repository root:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The root `Makefile` is a thin wrapper around CMake. Tests are built when libflint is the top-level project; the files under `tests/` also provide usage examples. Use `ctest --test-dir build --parallel` to run tests in parallel, or add `--label-exclude slow` to skip the slow macOS sampling test.

Compiler warnings are enabled by default for GCC and Clang (`FLINT_ENABLE_WARNINGS=ON`). To run with AddressSanitizer and UndefinedBehaviorSanitizer on a compiler that supports them:

```sh
cmake -S . -B build-sanitize -DCMAKE_BUILD_TYPE=Debug -DFLINT_SANITIZE=ON
cmake --build build-sanitize
ctest --test-dir build-sanitize --output-on-failure --label-exclude slow
```

## Using libflint with CMake

If the library is checked out at `lib/libflint` inside your project:

```cmake
add_subdirectory(lib/libflint)
target_link_libraries(your_target PRIVATE flint)
```

Replace `your_target` with an existing CMake target. The `flint` target exports its `include/` directory; include the headers for the modules you use, such as `lfvector.h` or `lfhashmap.h`. Its private compile definitions do not propagate to your project.

## Memory Management

You allocate the handle (`List`, `Vector`, and so on). The library allocates internal nodes and buffers. A `destroy` callback, if you pass one, frees owned payloads when the container is destroyed or cleared. Most pop and remove functions transfer the payload to you without calling that callback. Binary tree subtree removal also destroys payloads, and hash map replacement can destroy the old key and value; check the module page for the operation you use.

## Why the name 'libflint'?

`libflint` is named after my dog Flint, who passed away in 2021. I miss you buddy.

![flint](flinty.jpg)
