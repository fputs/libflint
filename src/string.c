#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lfstring.h"

size_t *find_substrings(const char* haystack, const char* needle, size_t *num_substrings) {
    size_t sz_h = strlen(haystack);
    size_t sz_n = strlen(needle);

    *num_substrings = 0;
    for (size_t i = 0; i <= sz_h - sz_n; ++i) {
        if (strncmp(haystack + i, needle, sz_n) == 0) {
            ++(*num_substrings);
        }
    }

    size_t *indicies = malloc(sizeof(size_t) * *num_substrings);
    if (indicies == NULL) {
        fprintf(stderr, "Memory allocation failed in find_substrings\n");
        return NULL;
    }

    size_t idx = 0;
    for (size_t i = 0; i <= sz_h - sz_n; ++i) {
        if (strncmp(haystack + i, needle, sz_n) == 0) {
            indicies[idx++] = i;
        }
    }

    return indicies;
}

const char* substr(const char* str, size_t idx, size_t len) {
   size_t sz_str = strlen(str);
   if (sz_str < len || idx + len > sz_str) {
       fprintf(stderr, "Improper size arguments in substr\n");
       return NULL;
   }

   char *substr = malloc(sizeof(char) * len + 1);
   if (substr == NULL) {
       fprintf(stderr, "Memory allocation error in substr\n");
       return NULL;
   }

   memcpy(substr, str + idx, len);
   substr[len] = '\0';

   return substr;
}
