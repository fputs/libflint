# libflint

My personal library of common C data structures and algorithms. Supports Linux, macOS, OpenBSD, and FreeBSD.

## Documentation

Extensive documentation can be found [here](https://fputs.com/docs/libflint/). You can also check out `tests/tests.c` to
see example usage from most of the library's API.

## Requirements

Building on Linux requires `libbsd`. Building on macOS, OpenBSD, or FreeBSD requires no extra dependencies.

## Libraries

`libflint` includes [uthash](https://github.com/troydhanson/uthash) for a hash table implementation. `uthash` is a single header file included in the source code of `libflint`. See the top of `include/uthash.h` for license information