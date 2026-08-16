#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lfstring.h"

static int size_add(size_t a, size_t b, size_t *out) {
    if (a > SIZE_MAX - b) {
        return -1;
    }
    *out = a + b;
    return 0;
}

static int size_mul(size_t a, size_t b, size_t *out) {
    if (b != 0 && a > SIZE_MAX / b) {
        return -1;
    }
    *out = a * b;
    return 0;
}

int str_find(const char* haystack, const char* needle, size_t *num_substrings, size_t **substrings) {
    if (*substrings != NULL) {
        return 1;
    }

    *num_substrings = 0;

    size_t sz_h = strlen(haystack);
    size_t sz_n = strlen(needle);
    if (sz_n > sz_h) {
        return 0;
    }

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

char* str_substr(const char* str, size_t idx, size_t len) {
   if (str == NULL) {
       return NULL;
   }

   size_t sz_str = strlen(str);
   if (len > sz_str || idx > sz_str - len) {
       return NULL;
   }

   size_t alloc;
   if (size_add(len, 1, &alloc) != 0) {
       return NULL;
   }
   char *sub = malloc(alloc);
   if (sub == NULL) {
       return NULL;
   }

   memcpy(sub, str + idx, len);
   sub[len] = '\0';

   return sub;
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
    if (sep == NULL || (count != 0 && strs == NULL)) {
        return NULL;
    }
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
        if (strs[i] == NULL) {
            return NULL;
        }
        if (size_add(total, strlen(strs[i]), &total) != 0) {
            return NULL;
        }
    }
    if (count > 1) {
        size_t seps;
        if (size_mul(sep_len, count - 1, &seps) != 0 ||
            size_add(total, seps, &total) != 0) {
            return NULL;
        }
    }

    size_t alloc;
    if (size_add(total, 1, &alloc) != 0) {
        return NULL;
    }
    char *out = malloc(alloc);
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
    if (str == NULL || old == NULL || new == NULL) {
        return NULL;
    }
    size_t old_len = strlen(old);
    size_t new_len = strlen(new);
    if (old_len == 0) {
        return strdup(str);
    }

    /* count occurrences */
    size_t count = 0;
    const char *p = str;
    while ((p = strstr(p, old)) != NULL) {
        if (count == SIZE_MAX) {
            return NULL;
        }
        count++;
        p += old_len;
    }

    size_t slen = strlen(str);
    size_t out_len;
    if (new_len >= old_len) {
        size_t extra;
        if (size_mul(count, new_len - old_len, &extra) != 0 ||
            size_add(slen, extra, &out_len) != 0) {
            return NULL;
        }
    } else {
        size_t extra;
        if (size_mul(count, old_len - new_len, &extra) != 0 || slen < extra) {
            return NULL;
        }
        out_len = slen - extra;
    }
    size_t alloc;
    if (size_add(out_len, 1, &alloc) != 0) {
        return NULL;
    }
    char *out = malloc(alloc);
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

char **str_split(const char *str, const char *delim, size_t *count) {
    *count = 0;
    size_t delim_len = strlen(delim);

    if (delim_len == 0) {
        char **result = malloc(sizeof(char *));
        if (result == NULL) {
            return NULL;
        }
        result[0] = strdup(str);
        if (result[0] == NULL) {
            free(result);
            return NULL;
        }
        *count = 1;
        return result;
    }

    /* count segments */
    size_t n = 1;
    const char *p = str;
    while ((p = strstr(p, delim)) != NULL) {
        n++;
        p += delim_len;
    }

    char **result = malloc(sizeof(char *) * n);
    if (result == NULL) {
        return NULL;
    }

    size_t i = 0;
    p = str;
    const char *next;
    while ((next = strstr(p, delim)) != NULL) {
        size_t seg_len = next - p;
        result[i] = malloc(seg_len + 1);
        if (result[i] == NULL) {
            for (size_t j = 0; j < i; j++) {
                free(result[j]);
            }
            free(result);
            return NULL;
        }
        memcpy(result[i], p, seg_len);
        result[i][seg_len] = '\0';
        i++;
        p = next + delim_len;
    }

    /* last segment */
    result[i] = strdup(p);
    if (result[i] == NULL) {
        for (size_t j = 0; j < i; j++) {
            free(result[j]);
        }
        free(result);
        return NULL;
    }
    *count = n;
    return result;
}

int str_contains(const char *str, const char *substr) {
    return strstr(str, substr) != NULL;
}
