Overflow-checked array allocation used by the containers. Available on every supported platform.

## Usage

Call `lf_reallocarray()` with the element count and element size. Pass `NULL` to allocate a new array.

```c
int *values = lf_reallocarray(NULL, 16, sizeof(int));
if (values != NULL) {
    int *grown = lf_reallocarray(values, 32, sizeof(int));
    if (grown != NULL) {
        values = grown;
    }
    free(values);
}
```

For a nonzero allocation size, a failed call leaves the original allocation yours to free. Store the result in a temporary pointer when growing an existing array.

## Functions

### lf_reallocarray

Reallocates memory for `nmemb` elements of `size` bytes each. Checks for multiplication overflow before calling `realloc()`. Returns `NULL` on overflow or allocation failure and sets `errno` to `ENOMEM`. A zero count or element size follows the platform's `realloc(ptr, 0)` behavior.

The prefixed name avoids conflicts with libc, libbsd, and SDK declarations of `reallocarray()`.

```c
void *lf_reallocarray(void *ptr, size_t nmemb, size_t size);
```
