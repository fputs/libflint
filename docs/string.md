C string helpers. All public names start with `lfstr_`.

Predicates that do not allocate take an `LfStr` view (`{const char *p; size_t n}`). The C-string wrappers (`lfstr_starts_with`, `lfstr_contains`, `lfstr_trim`, and so on) call those views for you. Functions that build a new string allocate from the heap, or from an `ArenaAllocator` if you use the `*_arena` variant. Free heap results with `free` or `lfstr_split_free`. Do not free arena results; they are released when the arena is cleared or freed.

## Types

```c
typedef struct {
    const char *p;
    size_t n;
} LfStr;
```

`LfStr` does not own `p`. `lfstr_from(NULL)` is `{NULL, 0}`.

```c
LfStr lfstr_from(const char *s);
LfStr lfstr_trim_view(LfStr s);
int lfstr_starts_with_view(LfStr s, LfStr prefix);
int lfstr_ends_with_view(LfStr s, LfStr suffix);
int lfstr_contains_view(LfStr s, LfStr needle);
```

`lfstr_trim_view` returns a slice of the same buffer. An empty needle / prefix / suffix matches.

## Functions

### lfstr_find

Finds the indices of all locations of the given needle in the haystack. `substrings` is an array of `size_t` indices that the substring is found at. This function allocates the memory for `substrings` and memory cleanup is managed by the user. `substrings` must be initialized to `NULL` before calling. `num_substrings` is a pointer to a pre-allocated `size_t` that will be modified to contain the number of found substrings and subsequently the size of `substrings`.

If no substrings are found, `substrings` will not be allocated and left set to `NULL`, `num_substrings` will be `0`, and the function will return `0`.

An empty needle matches every position, including one past the last character (`"abc"` + `""` yields `{0, 1, 2, 3}`).

Returns `0` on success, `1` if `substrings` is not `NULL` (already allocated), `-1` on allocation failure or if any pointer argument is `NULL`.

```c
int lfstr_find(const char *haystack, const char *needle,
               size_t *num_substrings, size_t **substrings);
int lfstr_find_arena(ArenaAllocator *arena, const char *haystack,
                     const char *needle, size_t *num_substrings,
                     size_t **substrings);

/* Usage */
const char *haystack = "One two three two";
const char *needle = "two";

size_t subs_sz = 0;
size_t *subs = NULL;
lfstr_find(haystack, needle, &subs_sz, &subs);
// subs: [ 4, 14 ]
// subs_sz: 2

free(subs);
```

### lfstr_substr

Extracts a substring at a specific index and length. This function returns a copy of the substring in a heap allocated buffer that the user is responsible for freeing. Returns `NULL` if there is an error, including when `idx + len` would wrap `size_t`.

```c
char *lfstr_substr(const char *str, size_t idx, size_t len);
char *lfstr_substr_arena(ArenaAllocator *arena, const char *str,
                         size_t idx, size_t len);

/* Usage */
const char *s = lfstr_substr("One two three", 4, 3);
assert(strcmp(s, "two") == 0);
free(s);
```

### lfstr_trim

Returns a new heap-allocated string with leading and trailing whitespace removed. The caller is responsible for freeing the returned string. For a non-allocating slice, use `lfstr_trim_view`.

```c
char *lfstr_trim(const char *str);
char *lfstr_trim_arena(ArenaAllocator *arena, const char *str);

/* Usage */
char *s = lfstr_trim("  hello  ");
// s: "hello"
free(s);
```

### lfstr_join

Joins an array of strings with a separator. Returns a new heap-allocated string. The caller is responsible for freeing the returned string. Returns an empty string if `count` is 0. Returns `NULL` if length math would wrap `size_t`.

```c
char *lfstr_join(const char **strs, size_t count, const char *sep);
char *lfstr_join_arena(ArenaAllocator *arena, const char **strs,
                       size_t count, const char *sep);

/* Usage */
const char *parts[] = {"foo", "bar", "baz"};
char *s = lfstr_join(parts, 3, ", ");
// s: "foo, bar, baz"
free(s);
```

### lfstr_starts_with

Returns `1` if `str` starts with `prefix`, `0` otherwise. `NULL` arguments return `0`.

```c
int lfstr_starts_with(const char *str, const char *prefix);
```

### lfstr_ends_with

Returns `1` if `str` ends with `suffix`, `0` otherwise.

```c
int lfstr_ends_with(const char *str, const char *suffix);
```

### lfstr_replace

Replaces all occurrences of `old` with `replacement` in `str`. Returns a new heap-allocated string. The caller is responsible for freeing the returned string. If `old` is not found, returns a copy of the original string. Returns `NULL` if length math would wrap `size_t`. The replacement parameter is named `replacement` so the header can be included from C++.

```c
char *lfstr_replace(const char *str, const char *old, const char *replacement);
char *lfstr_replace_arena(ArenaAllocator *arena, const char *str,
                          const char *old, const char *replacement);

/* Usage */
char *s = lfstr_replace("foo bar foo", "foo", "baz");
// s: "baz bar baz"
free(s);
```

### lfstr_to_upper

Returns a new heap-allocated string with all characters converted to uppercase. The caller is responsible for freeing the returned string.

```c
char *lfstr_to_upper(const char *str);
char *lfstr_to_upper_arena(ArenaAllocator *arena, const char *str);
```

### lfstr_to_lower

Returns a new heap-allocated string with all characters converted to lowercase. The caller is responsible for freeing the returned string.

```c
char *lfstr_to_lower(const char *str);
char *lfstr_to_lower_arena(ArenaAllocator *arena, const char *str);
```

### lfstr_split / lfstr_split_free

Splits a string into an array of strings based on a delimiter. Returns a heap-allocated array of heap-allocated strings. Free the result with `lfstr_split_free`. `count` is set to the number of parts.

```c
char **lfstr_split(const char *str, const char *delim, size_t *count);
char **lfstr_split_arena(ArenaAllocator *arena, const char *str,
                         const char *delim, size_t *count);
void lfstr_split_free(char **parts, size_t count);

/* Usage */
size_t count = 0;
char **parts = lfstr_split("foo,bar,baz", ",", &count);
// count: 3, parts: ["foo", "bar", "baz"]
lfstr_split_free(parts, count);
```

### lfstr_contains

Returns `1` if `substr` is found within `str`, `0` otherwise.

```c
int lfstr_contains(const char *str, const char *substr);
```
