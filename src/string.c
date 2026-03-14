#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

char *str_trim(const char *str) {
    const char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    const char *end = str + strlen(str);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    size_t len = end - start;
    char *out = malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    memcpy(out, start, len);
    out[len] = '\0';
    return out;
}

char *str_join(const char **strs, size_t count, const char *sep) {
    if (count == 0) {
        char *out = malloc(1);
        if (out != NULL) {
            out[0] = '\0';
        }
        return out;
    }
    size_t sep_len = strlen(sep);
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        total += strlen(strs[i]);
    }
    total += sep_len * (count - 1);

    char *out = malloc(total + 1);
    if (out == NULL) {
        return NULL;
    }
    char *p = out;
    for (size_t i = 0; i < count; i++) {
        size_t len = strlen(strs[i]);
        memcpy(p, strs[i], len);
        p += len;
        if (i < count - 1) {
            memcpy(p, sep, sep_len);
            p += sep_len;
        }
    }
    *p = '\0';
    return out;
}

int str_starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

int str_ends_with(const char *str, const char *suffix) {
    size_t str_len = strlen(str);
    size_t suf_len = strlen(suffix);
    if (suf_len > str_len) {
        return 0;
    }
    return strcmp(str + str_len - suf_len, suffix) == 0;
}

char *str_replace(const char *str, const char *old, const char *new) {
    size_t old_len = strlen(old);
    size_t new_len = strlen(new);
    if (old_len == 0) {
        return strdup(str);
    }

    /* count occurrences */
    size_t count = 0;
    const char *p = str;
    while ((p = strstr(p, old)) != NULL) {
        count++;
        p += old_len;
    }

    size_t out_len = strlen(str) + count * (new_len - old_len);
    char *out = malloc(out_len + 1);
    if (out == NULL) {
        return NULL;
    }

    char *w = out;
    p = str;
    while (*p) {
        if (strncmp(p, old, old_len) == 0) {
            memcpy(w, new, new_len);
            w += new_len;
            p += old_len;
        } else {
            *w++ = *p++;
        }
    }
    *w = '\0';
    return out;
}

char *str_to_upper(const char *str) {
    size_t len = strlen(str);
    char *out = malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < len; i++) {
        out[i] = toupper((unsigned char)str[i]);
    }
    out[len] = '\0';
    return out;
}

char *str_to_lower(const char *str) {
    size_t len = strlen(str);
    char *out = malloc(len + 1);
    if (out == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < len; i++) {
        out[i] = tolower((unsigned char)str[i]);
    }
    out[len] = '\0';
    return out;
}
