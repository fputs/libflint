# string

C string helpers

## Functions

### find_substrings
Finds the indicies of all locations of the given needle in the haystack. `substrings` is an array of `size_t` indicies that
the substring is found at. This function allocates the memory for `substrings` and memory cleanup is managed by the user. 
`num_substrings` is a pointer to a pre-allocated `size_t` that will be modified to contain the number of found substrings 
and subsequently the size of `substrings`.

If no substrings are found, `substrings` will not be allocated and left set to `NULL`, `num_substrings` will be `0`, and
the function will return `0`.

Returns 0 if the function is successful. Returns a non-zero value if there is an error.

```c
int find_substrings(const char* haystack, const char* needle, size_t *num_substrings, size_t **substrings);

/* Usage */
const char* haystack = "One two three two";
const char* needle = "two";

size_t subs_sz = 0;
size_t *subs = NULL;
size_t *subs = find_substrings(haystack, needle, &subs_sz, &subs);
// subs: [ 4, 14 ]
// subs_sz: 2

free(subs);
```

### substr

Extracts a substring at a specific index and length. This function returns a copy of the substring in a heap allocated
buffer that the user is responsible for freeing. Returns `NULL` if there is an error.

```c
const char* substr(const char* str, size_t idx, size_t len);

/* Usage */
const char *s = substr("One two three", 4, 3);
assert(strcmp(s, "two") == 0);
free(s);
```