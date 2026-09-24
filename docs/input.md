Read a regular, seekable file into memory. These functions use `fseek`/`ftell`, so pipes and other non-seekable paths are not supported.

## Usage

Read lines into an `LfLines` handle. Check the return value before using the result, then release it with `inp_lines_free`.

```c
LfLines lines;
if (inp_get_lines("data.txt", &lines) == 0) {
    for (size_t i = 0; i < lines.n; i++) {
        printf("%s\n", lines.lines[i]);
    }
    inp_lines_free(&lines);
}
```

`LfLines` holds a table of line or split pieces. The delimiter is a whole string. Empty tokens are kept, and the source string is not modified. `heap` is 1 when the table was allocated with `malloc`; call `inp_lines_free` to release it.

`*_arena` variants allocate from an `ArenaAllocator` when it is non-NULL. `heap` is then 0, and `inp_lines_free` only zeros the struct. The data remains valid until the arena is reset or destroyed. Passing NULL as the arena uses heap allocation instead. Do not call `free` on arena results.

`inp_get_lines` strips a trailing `\r` from each line so `\r\n` and `\n` both work. Bare `\r` is not treated as a line break. An empty file succeeds with `n == 0`. A missing file returns `-1`.

## Structs

### LfLines

A table of owned, NUL-terminated strings.

```c
typedef struct {
    char *base;
    char **lines;
    size_t n;
    int heap;
} LfLines;
```

Members:

- `base`: Additional backing allocation, currently NULL for results from this module
- `lines`: Table of string pointers; use `n` as the count
- `n`: Number of strings in the table
- `heap`: Nonzero for heap allocations, zero for arena allocations

## Functions

### inp_get_binary

Reads a file at `path` and returns the contents as an `unsigned char *` buffer. `fsz` receives the length. `fsz` must not be NULL. An empty file returns a non-NULL buffer with `*fsz == 0`. A missing file returns NULL. The caller frees the buffer.

```c
unsigned char *inp_get_binary(const char *path, size_t *fsz);
unsigned char *inp_get_binary_arena(ArenaAllocator *arena, const char *path,
                                    size_t *fsz);

/* Usage */
size_t fsz = 0;
unsigned char *buf = inp_get_binary("data.bin", &fsz);
free(buf);
```

### inp_get_input

Reads a file at `path` and returns the contents as a single NUL-terminated string. The caller frees it. Missing file returns NULL.

```c
char *inp_get_input(const char *path);
char *inp_get_input_arena(ArenaAllocator *arena, const char *path);

/* Usage */
char *str = inp_get_input("data.txt");
free(str);
```

### inp_get_lines

Reads a regular file at `path` and splits on `\n` into `out`. Trailing `\r` is stripped. A trailing newline does not produce an extra empty line; blank lines in the middle are kept. Returns `0` on success, including an empty file (`out->n == 0`). Returns `-1` on error (NULL arguments, missing file, I/O). On failure, `out` is unchanged. Release any previous result before reusing a handle for a successful call. Text after an embedded NUL is not included.

```c
int inp_get_lines(const char *path, LfLines *out);
int inp_get_lines_arena(ArenaAllocator *arena, const char *path, LfLines *out);
```

### inp_lines_free

Frees a heap-allocated table (`heap == 1`) and zeros the struct. Safe on NULL. Use this after `inp_get_lines` and `inp_split`. Arena-backed results have `heap == 0` and need no `free`.

```c
void inp_lines_free(LfLines *lines);
```

### inp_get_ints

Reads a newline-separated list of integers. The caller frees the returned array with `free()`. `sz` receives the count. Returns `NULL` if `path` or `sz` is NULL, the file is missing, or a line is not a decimal integer in the `int` range. Leading whitespace and a sign are accepted; trailing whitespace and blank lines are rejected. An empty file returns a freeable pointer with `*sz == 0`.

```c
int *inp_get_ints(const char *path, size_t *sz);
int *inp_get_ints_arena(ArenaAllocator *arena, const char *path, size_t *sz);

/* Usage */
size_t sz = 0;
int *nums = inp_get_ints("nums.txt", &sz);
if (nums != NULL) {
    for (size_t i = 0; i < sz; i++) {
        printf("%d\n", nums[i]);
    }
    free(nums);
}
```

### inp_split

Splits `s` on the string delimiter `delim`. Does not modify `s`. Empty tokens are kept (`"a,,b"` split on `","` is three parts). An empty delimiter returns the whole string as one token. Returns `0` on success or `-1` on error, leaving `out` unchanged on failure.

```c
int inp_split(const char *s, const char *delim, LfLines *out);
int inp_split_arena(ArenaAllocator *arena, const char *s, const char *delim,
                    LfLines *out);

/* Usage */
LfLines sp;
if (inp_split("Split on whitespace", " ", &sp) == 0) {
    printf("%s\n", sp.lines[0]); /* Prints "Split" */
    inp_lines_free(&sp);
}
```

Process helpers (`lf_capture_system`) live in [[process]], not this header.
