#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lfcompat.h"
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

LfStr lfstr_from(const char *s) {
    LfStr v;
    if (s == NULL) {
        v.p = NULL;
        v.n = 0;
        return v;
    }
    v.p = s;
    v.n = strlen(s);
    return v;
}

LfStr lfstr_trim_view(LfStr s) {
    LfStr v = {NULL, 0};
    if (s.p == NULL) {
        return v;
    }
    const char *start = s.p;
    const char *end = s.p + s.n;
    while (start < end && isspace((unsigned char)*start)) {
        start++;
    }
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    v.p = start;
    v.n = (size_t)(end - start);
    return v;
}

int lfstr_starts_with_view(LfStr s, LfStr prefix) {
    if (prefix.n == 0) {
        return 1;
    }
    if (s.p == NULL || prefix.p == NULL || prefix.n > s.n) {
        return 0;
    }
    return memcmp(s.p, prefix.p, prefix.n) == 0;
}

int lfstr_ends_with_view(LfStr s, LfStr suffix) {
    if (suffix.n == 0) {
        return 1;
    }
    if (s.p == NULL || suffix.p == NULL || suffix.n > s.n) {
        return 0;
    }
    return memcmp(s.p + (s.n - suffix.n), suffix.p, suffix.n) == 0;
}

int lfstr_contains_view(LfStr s, LfStr needle) {
    if (needle.n == 0) {
        return 1;
    }
    if (s.p == NULL || needle.p == NULL || needle.n > s.n) {
        return 0;
    }
    const char *end = s.p + (s.n - needle.n);
    for (const char *p = s.p; p <= end; p++) {
        if (memcmp(p, needle.p, needle.n) == 0) {
            return 1;
        }
    }
    return 0;
}

int lfstr_find(const char *haystack, const char *needle,
               size_t *num_substrings, size_t **substrings) {
    if (haystack == NULL || needle == NULL ||
        num_substrings == NULL || substrings == NULL) {
        return -1;
    }
    if (*substrings != NULL) {
        return 1;
    }

    *num_substrings = 0;

    size_t sz_h = strlen(haystack);
    size_t sz_n = strlen(needle);
    if (sz_n > sz_h) {
        return 0;
    }

    /* Empty needle matches every position, including one past the last char. */
    for (size_t i = 0; i <= sz_h - sz_n; ++i) {
        if (strncmp(haystack + i, needle, sz_n) == 0) {
            ++(*num_substrings);
        }
    }

    if (*num_substrings == 0) {
        return 0;
    }

    *substrings = lf_reallocarray(NULL, *num_substrings, sizeof(size_t));
    if (*substrings == NULL) {
        *num_substrings = 0;
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

char *lfstr_substr(const char *str, size_t idx, size_t len) {
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

char *lfstr_trim(const char *str) {
    if (str == NULL) {
        return NULL;
    }
    LfStr v = lfstr_trim_view(lfstr_from(str));
    size_t alloc;
    if (size_add(v.n, 1, &alloc) != 0) {
        return NULL;
    }
    char *out = malloc(alloc);
    if (out == NULL) {
        return NULL;
    }
    memcpy(out, v.p, v.n);
    out[v.n] = '\0';
    return out;
}

char *lfstr_join(const char **strs, size_t count, const char *sep) {
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

int lfstr_starts_with(const char *str, const char *prefix) {
    if (str == NULL || prefix == NULL) {
        return 0;
    }
    return lfstr_starts_with_view(lfstr_from(str), lfstr_from(prefix));
}

int lfstr_ends_with(const char *str, const char *suffix) {
    if (str == NULL || suffix == NULL) {
        return 0;
    }
    return lfstr_ends_with_view(lfstr_from(str), lfstr_from(suffix));
}

char *lfstr_replace(const char *str, const char *old, const char *replacement) {
    if (str == NULL || old == NULL || replacement == NULL) {
        return NULL;
    }
    size_t old_len = strlen(old);
    size_t new_len = strlen(replacement);
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
            memcpy(w, replacement, new_len);
            w += new_len;
            p += old_len;
        } else {
            *w++ = *p++;
        }
    }
    *w = '\0';
    return out;
}

char *lfstr_to_upper(const char *str) {
    if (str == NULL) {
        return NULL;
    }
    size_t len = strlen(str);
    size_t alloc;
    if (size_add(len, 1, &alloc) != 0) {
        return NULL;
    }
    char *out = malloc(alloc);
    if (out == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < len; i++) {
        out[i] = (char)toupper((unsigned char)str[i]);
    }
    out[len] = '\0';
    return out;
}

char *lfstr_to_lower(const char *str) {
    if (str == NULL) {
        return NULL;
    }
    size_t len = strlen(str);
    size_t alloc;
    if (size_add(len, 1, &alloc) != 0) {
        return NULL;
    }
    char *out = malloc(alloc);
    if (out == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < len; i++) {
        out[i] = (char)tolower((unsigned char)str[i]);
    }
    out[len] = '\0';
    return out;
}

char **lfstr_split(const char *str, const char *delim, size_t *count) {
    if (str == NULL || delim == NULL || count == NULL) {
        return NULL;
    }
    *count = 0;
    size_t delim_len = strlen(delim);

    if (delim_len == 0) {
        char **result = lf_reallocarray(NULL, 1, sizeof(char *));
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
        if (n == SIZE_MAX) {
            return NULL;
        }
        n++;
        p += delim_len;
    }

    char **result = lf_reallocarray(NULL, n, sizeof(char *));
    if (result == NULL) {
        return NULL;
    }

    size_t i = 0;
    p = str;
    const char *next;
    while ((next = strstr(p, delim)) != NULL) {
        size_t seg_len = (size_t)(next - p);
        size_t alloc;
        if (size_add(seg_len, 1, &alloc) != 0) {
            lfstr_split_free(result, i);
            return NULL;
        }
        result[i] = malloc(alloc);
        if (result[i] == NULL) {
            lfstr_split_free(result, i);
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
        lfstr_split_free(result, i);
        return NULL;
    }
    *count = n;
    return result;
}

void lfstr_split_free(char **parts, size_t count) {
    if (parts == NULL) {
        return;
    }
    for (size_t i = 0; i < count; i++) {
        free(parts[i]);
    }
    free(parts);
}

int lfstr_contains(const char *str, const char *substr) {
    if (str == NULL || substr == NULL) {
        return 0;
    }
    return lfstr_contains_view(lfstr_from(str), lfstr_from(substr));
}
