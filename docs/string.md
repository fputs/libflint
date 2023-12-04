# string

C string helpers

## Functions

### find_substrings
Finds the indexes of all locations of the given needle in the haystack. The returned array of `size_t` is allocated by
this function and memory cleanup is managed by the user. `num_substrings` is a pointer to a pre-allocated `size_t` that will
be modified to contain the number of found substrings and subsequently the size of the returned array of `size_t`.
Returns `NULL` and prints to `stderr` if there is an error.

```c
size_t *find_substrings(const char* haystack, const char* needle, size_t *num_substrings);

/* Usage */
const char* haystack = "One two three two";
const char* needle = "two";
size_t subs_sz = 0;

size_t *subs = find_substrings(haystack, needle, &subs_sz);
// subs: [ 4, 14 ]
// subs_sz: 2

free(subs);
```

### substr

Extracts a substring at a specific index and length. This function returns a copy of the substring in a heap allocated
buffer that the user is responsible for freeing. Returns `NULL` and prints to `stderr` if there is an error.

```c
const char* substr(const char* str, size_t idx, size_t len);

/* Usage */
const char *s = substr("One two three", 4, 3);
assert(strcmp(s, "two") == 0);
free(s);
```