#include "lfstrbuf.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t capacity(size_t old, size_t need) {
    size_t n = old ? old : 16;
    while (n < need) {
        if (n > SIZE_MAX / 2) return need;
        n *= 2;
    }
    return n;
}

int lf_strbuf_reserve(LfStrBuf *b, size_t length) {
    if (!b || length == SIZE_MAX) return -1;
    if (length < b->cap) return 0;
    size_t cap = capacity(b->cap, length + 1);
    char *p = realloc(b->data, cap);
    if (!p) return -1;
    p[b->len] = '\0';
    b->data = p;
    b->cap = cap;
    return 0;
}

int lf_strbuf_init(LfStrBuf *b) {
    if (!b) return -1;
    memset(b, 0, sizeof(*b));
    return lf_strbuf_reserve(b, 0);
}

int lf_strbuf_append_view(LfStrBuf *b, LfStr text) {
    if (!b || (!text.p && text.n) || text.n > SIZE_MAX - b->len - 1) return -1;
    size_t len = b->len + text.n;
    if (len >= b->cap) {
        /* Keep the original allocation alive so self-appends survive growth. */
        size_t cap = capacity(b->cap, len + 1);
        char *p = malloc(cap);
        if (!p) return -1;
        if (b->len) memcpy(p, b->data, b->len);
        if (text.n) memcpy(p + b->len, text.p, text.n);
        free(b->data);
        b->data = p;
        b->cap = cap;
    } else if (text.n) {
        memmove(b->data + b->len, text.p, text.n);
    }
    b->len = len;
    b->data[len] = '\0';
    return 0;
}

int lf_strbuf_append(LfStrBuf *b, const char *text) {
    if (!text) return -1;
    return lf_strbuf_append_view(b, (LfStr){text, strlen(text)});
}

int lf_strbuf_vappendf(LfStrBuf *b, const char *fmt, va_list args) {
    if (!b || !fmt) return -1;
    va_list copy;
    va_copy(copy, args);
    int n = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    if (n < 0 || (uintmax_t)n >= SIZE_MAX || (size_t)n > SIZE_MAX - b->len - 1) return -1;
    char *text = malloc((size_t)n + 1);
    if (!text) return -1;
    va_copy(copy, args);
    int written = vsnprintf(text, (size_t)n + 1, fmt, copy);
    va_end(copy);
    int result = written == n ? lf_strbuf_append_view(b, (LfStr){text, (size_t)n}) : -1;
    free(text);
    return result;
}

int lf_strbuf_appendf(LfStrBuf *b, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int result = lf_strbuf_vappendf(b, fmt, args);
    va_end(args);
    return result;
}

void lf_strbuf_clear(LfStrBuf *b) {
    if (!b) return;
    b->len = 0;
    if (b->data) b->data[0] = '\0';
}

char *lf_strbuf_detach(LfStrBuf *b) {
    if (!b || lf_strbuf_reserve(b, b->len)) return NULL;
    char *p = b->data;
    memset(b, 0, sizeof(*b));
    return p;
}

void lf_strbuf_destroy(LfStrBuf *b) {
    if (!b) return;
    free(b->data);
    memset(b, 0, sizeof(*b));
}
