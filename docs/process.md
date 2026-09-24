Run a shell command and capture its standard output as a string.

## Usage

Call `lf_capture_system()` with a command and an initial buffer size. Pass `0` to use the default size. The buffer grows as output is read.

```c
char *cap = lf_capture_system("printf 'hello\\n'", 0);
if (cap != NULL) {
    printf("%s", cap);
    free(cap);
}
```

The returned string is yours to free. The command runs through `/bin/sh` without sanitizing; do not pass untrusted input.

## Functions

### lf_capture_system

Runs a command through `popen()` and returns its standard output as a heap allocated, NUL-terminated string. `buf_sz` is the initial buffer size; `0` uses `LF_CAPTURE_SYSTEM_BUFSIZE` (1024).

Returns `NULL` if `cmd` is NULL, `buf_sz` is 1, or starting the command, allocating memory, reading, or waiting for the child fails. A nonzero command exit status still returns the captured output. Standard error is not captured unless the command redirects it.

```c
char *lf_capture_system(const char *cmd, size_t buf_sz);
```
