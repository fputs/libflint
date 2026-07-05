I/O module to assist with consuming data from files. All functions use the `inp_` prefix.
## Functions

### inp_get_binary

Reads a file at `path` and returns the contents as an unsigned char* buffer. The `fsz` parameter stores the length of the
returned array. The buffer is allocated inside the function, so the user is responsible for freeing it when finished.

```c
unsigned char *inp_get_binary(const char *path, size_t *fsz);

/* Usage */
size_t fsz = 0;
unsigned char *buf = inp_get_binary("/home/evan/binfile", &fsz);
free(buf);
```

### inp_get_input

Reads a file at `path` and returns the contents as a single string. The string is allocated inside the function and
the user is responsible for freeing it when finished.

```c
char *inp_get_input(const char *path);

/* Usage */
char *str = inp_get_input("/home/evan/textfile");
free(str);
```

### inp_get_lines

Reads a file at `path` and returns the contents as an array of strings. The newline character `\n` is used as the
delimiter to determine where the file is split. The user is responsible for cleaning up the memory using `inp_del_lines()`.
`lsz` is set to the number of lines in the array.

```c
char **inp_get_lines(const char *path, size_t *lsz);

/* Usage */
size_t sz = 0;
char **lines = inp_get_lines("/home/evan/textfile", &sz);
for (size_t i = 0; i < sz; i++) {
    printf("%s\n", lines[i]);
}
inp_del_lines(lines);
```

### inp_del_lines

Frees all memory used by `inp_get_lines()`

```c
void inp_del_lines(char **lines);
```

### inp_get_ints
Reads a file at `path` and returns the contents as an array of integers. The file is assumed to be a newline seperated
list of integers and nothing else.

The newline character `\n` is used as the delimiter to determine where the file is split. The user is responsible for
cleaning up the memory using `free()`. `lsz` is set to the number of lines in the array. Returns `NULL` if input is `NULL`.

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

Takes a string `s` and splits it into an array of strings based on the delimiter. **`s` is modified in-place** by `strtok`, which inserts null terminators at delimiter positions. The user is responsible for cleaning up the memory of the split using `inp_del_split()`. `lsz` is set to the size of the split.

```c
char **inp_split(char *s, size_t *lsz, const char *delim);

/* Usage */
size_t sp_sz = 0;
char **sp = inp_split("Split on whitespace", &sp_sz, " ");
printf("%s\n", sp[0]); // Prints "Split"
```

### inp_del_split

Frees the array returned by `inp_split()`. Does not free the original string (which was modified in-place by `strtok`).

```c
void inp_del_split(char **sp);

/* Usage */
size_t sp_sz = 0;
char **sp = inp_split("Delete Me!", &sp_sz, " ");
inp_del_split(sp);
```

### inp_capture_system

Runs a command on the system shell and returns stdout as a string. Reads the full output in a loop. `buf_sz` is the initial buffer size — passing `0` will use the default buffer size of `1024`.

**Security warning:** The command string is passed directly to `/bin/sh` via `popen()` with no sanitization. Never pass untrusted or user-supplied input as `cmd` — shell metacharacters will be interpreted, which can lead to command injection.

User is responsible for freeing the returned string. Returns `NULL` if input is `NULL` or on error.

```c
char *inp_capture_system(const char *cmd, size_t buf_sz);

/* Usage */
char *cap = inp_capture_system("ls $HOME", 0);
printf("%s\n", cap);
free(cap);
```
