One-shot slurp of a regular, seekable file. All slurp functions use `fseek`/`ftell`, so pipes and other non-seekable paths are not supported.

`LfLines` is the explicit lines result. `base` is the slurped buffer when the function owns it (`inp_get_lines`), or `NULL` when the caller owns the mutable string (`inp_split`). `lines` points into that buffer. One destructor, `inp_lines_free`, frees both fields — there is no hidden `lines[-1]` pointer and no pair of `char **` destructors to swap.

`inp_get_lines` strips a trailing `\r` from each line so `\r\n` and `\n` both work. Bare `\r` line endings are not treated as breaks. An empty file succeeds with `n == 0`. A missing file returns `-1`.

## Structs

```c
typedef struct {
    char *base;
    char **lines;
    size_t n;
} LfLines;
```

## Functions

### inp_get_binary

Reads a file at `path` and returns the contents as an `unsigned char *` buffer. `fsz` receives the length. `fsz` must not be NULL. An empty file returns a non-NULL buffer with `*fsz == 0`. A missing file returns NULL. The caller frees the buffer.

```c
unsigned char *inp_get_binary(const char *path, size_t *fsz);

/* Usage */
size_t fsz = 0;
unsigned char *buf = inp_get_binary("/home/evan/binfile", &fsz);
free(buf);
```

### inp_get_input

Reads a file at `path` and returns the contents as a single NUL-terminated string. The caller frees it. Missing file returns NULL.

```c
char *inp_get_input(const char *path);

/* Usage */
char *str = inp_get_input("/home/evan/textfile");
free(str);
```

### inp_get_lines

Reads a regular file at `path` and splits on `\n` into `out`. Trailing `\r` is stripped. Returns `0` on success, including an empty file (`out->n == 0`). Returns `-1` on error (NULL arguments, missing file, I/O). Do not destroy `out` on failure.

```c
int inp_get_lines(const char *path, LfLines *out);

/* Usage */
LfLines lines;
if (inp_get_lines("/home/evan/textfile", &lines) == 0) {
    for (size_t i = 0; i < lines.n; i++) {
        printf("%s\n", lines.lines[i]);
    }
    inp_lines_free(&lines);
}
```

### inp_lines_free

Frees `lines->lines` and `lines->base` and zeros the struct. Safe on NULL. The same function is used after `inp_get_lines` and `inp_split`.

```c
void inp_lines_free(LfLines *lines);
```

### inp_get_ints

Reads a newline-separated list of integers. The caller frees the returned array with `free()`. `sz` receives the count. Returns `NULL` if `path` or `sz` is NULL, the file is missing, or a line is not an integer. An empty file returns a freeable pointer with `*sz == 0`.

```c
int *inp_get_ints(const char *path, size_t *sz);

/* Usage */
size_t sz = 0;
int *nums = inp_get_ints("/home/evan/intfile", &sz);
for (size_t i = 0; i < sz; i++) {
    printf("%d\n", nums[i]);
}
free(nums);
```

### inp_split

Splits a **mutable** buffer `s` on `delim` using `strtok` (which writes NULs). Do not pass a string literal. The caller keeps ownership of `s`; `out->base` is NULL. Returns `0` on success, `-1` on error.

```c
int inp_split(char *s, const char *delim, LfLines *out);

/* Usage */
char buf[] = "Split on whitespace";
LfLines sp;
inp_split(buf, " ", &sp);
printf("%s\n", sp.lines[0]); /* Prints "Split" */
inp_lines_free(&sp);
```

Process helpers (`lf_capture_system`) live in [[process]], not this header.
