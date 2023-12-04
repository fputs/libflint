#ifndef LIBFLINT_H_STRING
#define LIBFLINT_H_STRING

#include <stddef.h>

size_t *find_substrings(const char* haystack, const char* needle, size_t *num_substrings);

const char* substr(const char* str, size_t idx, size_t len);

#endif // LIBFLINT_H_STRING
