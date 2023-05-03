# lfinput

I/O module to assist with consuming data from files

## Functions

### get_binary

Reads a file at `path` and returns the contents as a char* buffer. The buffer is allocated inside the function and
the user is responsible for freeing it when finished.

```c
char *get_binary(const char *path);

/* Usage */
char *buf = get_binary("/home/evan/binfile");
free(buf);
```


### get_input

Reads a file at `path` and returns the contents as a single string. The string is allocated inside the function and
the user is responsible for freeing it when finished.

```c
char *get_input(const char *path);

/* Usage */
char *str = get_input("/home/evan/textfile");
free(str);
```

### get_lines

Reads a file at `path` and returns the contents as an array of strings. The newline character `\n` is used as the
delimiter to determine where the file is split. The user is responsible for cleaning up the memory using `del_lines()`.
`lsz` is set to the number of lines in the array.

```c
char **get_lines(const char *path, size_t *lsz);

/* Usage */
size_t sz = 0;
char **lines = get_lines("/home/evan/textfile", &sz);
for (size_t i = 0; i < sz; i++) {
    printf("%s\n", lines[i]);
}
del_lines(lines);
```

### del_lines

Frees all memory used by `get_lines()`

```c
void del_lines(char **lines);
```

### get_ints
Reads a file at `path` and returns the contents as an array of integers. The file is assumed to be a newline seperated
list of integers and nothing else.

The newline character `\n` is used as the delimiter to determine where the file is split. The user is responsible for
cleaning up the memory using `free()`. `lsz` is set to the number of lines in the array.

```c
int *get_ints(const char *path, size_t *lsz);

/* Usage */
int *nums = get_ints("/home/evan/intfile");
for (size_t i = 0; i < sz; i++) {
    printf("%d\n", nums[i]);
}
free(nums);
```

### split

Takes a string `s` and splits it into an array of strings based on the delimiter. `s` is left unchanged. The user is
responsible for cleaning up the memory of the split using `del_split()`. `sp_sz` is set to the size of the split.

```c
char **split(char *s, size_t *lsz, const char *delim)

/* Usage */
size_t sp_sz = 0;
char **sp = split("Split on whitespace", &sp_sz, " ");
printf("%s\n", sp[0]); // Prints "Split"
```

### del_split

Frees all memory used by `split()`. Just like `split`, it does not touch the original string
```c
void del_split(char **sp);

/* Usage */
size_t sp_sz = 0;
char **sp = split("Delete Me!", &sp_sz, " ");
void del_split(sp);
```
