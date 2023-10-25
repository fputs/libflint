# libflint

`libflint` is a library of common C data structures and algorithms, designed to be as agnostic as possible for its
users. Over time, it has become my personal library of reusable C code.

## Requirements

### Linux

Requires `libbsd`. This should be available on all major Linux distros.

### macOS, OpenBSD, FreeBSD

Requires no extra dependencies

## Memory Management

The parts of this library that create data structures pass all responsibility of memory management to the user. This
is clearly documented in the individual module's documentation.

## Why the name 'libflint'?

`libflint` is named after my dog Flint, who passed away in 2021. I miss you buddy.

![flint](flinty.jpg)