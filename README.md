# libflint

A C library of containers and helpers. Supports Linux, macOS, OpenBSD, and FreeBSD.

## Documentation

[Module documentation](https://burkey.co/Software+Documentation/libflint/libflint). The files under `tests/` are also usable as examples.

## Building

CMake is required. Use CMake directly, or the `Makefile`, which is a thin wrapper around it.

## CMake

```cmake
# If libflint is in ./lib/libflint
add_subdirectory(lib/libflint)

# when building a target...
target_link_libraries(${TARGET} PRIVATE flint)
```

The `flint` target exports `include/`, so you do not need a separate
`target_include_directories` for libflint headers. Compile definitions stay on
the `flint` target and are not added to the rest of your project.

## Testing

```sh
cmake -B build && cmake --build build
ctest --test-dir build                # run all tests
ctest --test-dir build --parallel     # run in parallel
ctest --test-dir build --label-exclude slow  # skip slow tests (macos)

# AddressSanitizer + UndefinedBehaviorSanitizer
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DFLINT_SANITIZE=ON
cmake --build build && ctest --test-dir build --output-on-failure -LE slow
```

## Requirements

A C99 compiler and CMake 3.17 or newer. Linux needs `libbsd` (`strtonum` in the input module). macOS, OpenBSD, and FreeBSD need no extra libraries.

## Libraries

Overflow-checked array allocation is provided as `lf_reallocarray` in `lfcompat.h` on every platform.
