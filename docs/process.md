Run a command and capture its stdout.

## Functions

### lf_capture_system

Runs a command through `/bin/sh` via `popen` and returns stdout as a heap string. `buf_sz` is the initial buffer size; `0` uses `LF_CAPTURE_SYSTEM_BUFSIZE` (1024).

The command string is not sanitized. Do not pass untrusted input; `/bin/sh` interprets metacharacters.

The caller frees the returned string. Returns `NULL` if `cmd` is NULL or on error.

```c
char *lf_capture_system(const char *cmd, size_t buf_sz);

/* Usage */
char *cap = lf_capture_system("ls $HOME", 0);
printf("%s\n", cap);
free(cap);
```
