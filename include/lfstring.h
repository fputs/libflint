#ifndef LIBFLINT_H_STRING
#define LIBFLINT_H_STRING

#include <stddef.h>

int find_substrings(const char* haystack, const char* needle, size_t *num_substrings, size_t **substrings);

char* substr(const char* str, size_t idx, size_t len);

char *str_trim(const char *str);

char *str_join(const char **strs, size_t count, const char *sep);

int str_starts_with(const char *str, const char *prefix);

int str_ends_with(const char *str, const char *suffix);

char *str_replace(const char *str, const char *old, const char *new);

char *str_to_upper(const char *str);

char *str_to_lower(const char *str);

#endif // LIBFLINT_H_STRING
