#include <string.h>
#include <stdlib.h>

#include "lfstring.h"

int find_substrings(const char* haystack, const char* needle, size_t *num_substrings, size_t **substrings) {
    if (*substrings != NULL) {
        return 1;
    }

    size_t sz_h = strlen(haystack);
    size_t sz_n = strlen(needle);
    if ((int)sz_h - (int)sz_n < 0) {
        return 0;
    }

    *num_substrings = 0;
    for (size_t i = 0; i <= sz_h - sz_n; ++i) {
        if (strncmp(haystack + i, needle, sz_n) == 0) {
            ++(*num_substrings);
        }
    }

    if (*num_substrings == 0) {
        return 0;
    }

    *substrings = malloc(sizeof(size_t) * *num_substrings);
    if (*substrings == NULL) {
        return -1;
    }

    size_t idx = 0;
    for (size_t i = 0; i <= sz_h - sz_n; ++i) {
        if (strncmp(haystack + i, needle, sz_n) == 0) {
            (*substrings)[idx++] = i;
        }
    }

    return 0;
}

char* substr(const char* str, size_t idx, size_t len) {
   size_t sz_str = strlen(str);
   if (sz_str < len || idx + len > sz_str) {
       return NULL;
   }

   char *substr = malloc(sizeof(char) * (len + 1));
   if (substr == NULL) {
       return NULL;
   }

   memcpy(substr, str + idx, len);
   substr[len] = '\0';

   return substr;
}
