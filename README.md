# libflint

My personal library of common C data structures and algorithms. Supports Linux, macOS, OpenBSD, and FreeBSD.

## Documentation

[Extensive documentation can be found here](https://burkey.co/Software+Documentation/libflint/libflint). You can also check out the per-module test files in `tests/` for example usage.

## Building

`libflint` requires Cmake to be built. It can then be built using either `cmake` or `make` with the provided build files. The `Makefile` is a wrapper around Cmake for Makefile compatibility.

## Cmake

```cmake
# If libflint is in ./lib/libflint
add_subdirectory(lib/libflint)

# when building a target...
target_link_libraries(${TARGET} PRIVATE flint)
```

`flint` exports its `include/` directory, so consumers do not need a separate
`target_include_directories` for libflint headers. Compile definitions stay on
the `flint` target and are not injected into the parent project.

## Testing

```sh
cmake -B build && cmake --build build
ctest --test-dir build                # run all tests
ctest --test-dir build --parallel     # run in parallel
ctest --test-dir build --label-exclude slow  # skip slow tests (network, macos)
```

## Requirements

Building on Linux requires `libbsd`. Building on macOS, OpenBSD, or FreeBSD requires no extra dependencies.

## Libraries

`libflint` includes [uthash](https://github.com/troydhanson/uthash) for a hash table implementation. `uthash` is a single header file included in the source code of `libflint`. See the top of `include/uthash.h` for license information
