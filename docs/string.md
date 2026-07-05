C string helpers
## Functions

### str_find
Finds the indices of all locations of the given needle in the haystack. `substrings` is an array of `size_t` indices that the substring is found at. This function allocates the memory for `substrings` and memory cleanup is managed by the user. `substrings` must be initialized to `NULL` before calling. `num_substrings` is a pointer to a pre-allocated `size_t` that will be modified to contain the number of found substrings and subsequently the size of `substrings`.

If no substrings are found, `substrings` will not be allocated and left set to `NULL`, `num_substrings` will be `0`, and the function will return `0`.

Returns `0` on success, `1` if `substrings` is not `NULL` (already allocated), `-1` on allocation failure.

```c
int str_find(const char* haystack, const char* needle, size_t *num_substrings, size_t **substrings);

/* Usage */
const char* haystack = "One two three two";
const char* needle = "two";

size_t subs_sz = 0;
size_t *subs = NULL;
str_find(haystack, needle, &subs_sz, &subs);
// subs: [ 4, 14 ]
// subs_sz: 2

free(subs);
```

### str_substr

Extracts a substring at a specific index and length. This function returns a copy of the substring in a heap allocated buffer that the user is responsible for freeing. Returns `NULL` if there is an error.

```c
char* str_substr(const char* str, size_t idx, size_t len);

/* Usage */
const char *s = str_substr("One two three", 4, 3);
assert(strcmp(s, "two") == 0);
free(s);
```

### str_trim

Returns a new heap-allocated string with leading and trailing whitespace removed. The caller is responsible for freeing the returned string.

```c
char *str_trim(const char *str);

/* Usage */
char *s = str_trim("  hello  ");
// s: "hello"
free(s);
```

### str_join

Joins an array of strings with a separator. Returns a new heap-allocated string. The caller is responsible for freeing the returned string. Returns an empty string if `count` is 0.

```c
char *str_join(const char **strs, size_t count, const char *sep);

/* Usage */
const char *parts[] = {"foo", "bar", "baz"};
char *s = str_join(parts, 3, ", ");
// s: "foo, bar, baz"
free(s);
```

### str_starts_with

Returns `1` if `str` starts with `prefix`, `0` otherwise.

```c
int str_starts_with(const char *str, const char *prefix);
```

### str_ends_with

Returns `1` if `str` ends with `suffix`, `0` otherwise.

```c
int str_ends_with(const char *str, const char *suffix);
```

### str_replace

Replaces all occurrences of `old` with `new` in `str`. Returns a new heap-allocated string. The caller is responsible for freeing the returned string. If `old` is not found, returns a copy of the original string.

```c
char *str_replace(const char *str, const char *old, const char *new);

/* Usage */
char *s = str_replace("foo bar foo", "foo", "baz");
// s: "baz bar baz"
free(s);
```

### str_to_upper

Returns a new heap-allocated string with all characters converted to uppercase. The caller is responsible for freeing the returned string.

```c
char *str_to_upper(const char *str);
```

### str_to_lower

Returns a new heap-allocated string with all characters converted to lowercase. The caller is responsible for freeing the returned string.

```c
char *str_to_lower(const char *str);
```

### str_split

Splits a string into an array of strings based on a delimiter. Returns a heap-allocated array of heap-allocated strings. The caller is responsible for freeing the returned array and its elements. `count` is set to the number of parts.

```c
char **str_split(const char *str, const char *delim, size_t *count);

/* Usage */
size_t count = 0;
char **parts = str_split("foo,bar,baz", ",", &count);
// count: 3, parts: ["foo", "bar", "baz"]
for (size_t i = 0; i < count; i++) free(parts[i]);
free(parts);
```

### str_contains

Returns `1` if `substr` is found within `str`, `0` otherwise.

```c
int str_contains(const char *str, const char *substr);
```