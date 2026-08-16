Portable overflow-checked allocation used by containers. Not a Darwin grab-bag — every platform compiles `src/compat.c`.

## Functions

### lf_reallocarray

Reallocates memory for an array of `nmemb` elements of `size` bytes each. Checks for multiplication overflow before calling `realloc`. Returns `NULL` on overflow or allocation failure and sets `errno` to `ENOMEM`.

This is the library's `reallocarray` equivalent. The name is prefixed so it does not clash with libc, libbsd, or newer SDKs.

```c
void *lf_reallocarray(void *ptr, size_t nmemb, size_t size);
```
