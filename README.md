# libflint

My personal library of common C data structures and algorithms. Supports Linux, macOS, OpenBSD, and FreeBSD.

## Documentation

Extensive documentation can be found [here](https://wiki.burkey.co/doku.php?id=libflint:start). You can also check out `tests/tests.c` to
see example usage from most of the library's API.

## Building

`libflint` requires Cmake to be built. It can then be built using either `cmake` or `make` with the provided build files. The `Makefile` is a wrapper around Cmake for Makefile compatibility.

### Cmake

```cmake
# If libflint is in ./lib/libflint
add_subdirectory(lib/libflint)

# when building target...
target_include_directories(${TARGET} PRIVATE lib/libflint/include)
target_link_libraries(${TARGET} PRIVATE flint )
```

## Requirements

Building on Linux requires `libbsd`. Building on macOS, OpenBSD, or FreeBSD requires no extra dependencies.

## Libraries

`libflint` includes [uthash](https://github.com/troydhanson/uthash) for a hash table implementation. `uthash` is a single header file included in the source code of `libflint`. See the top of `include/uthash.h` for license information
