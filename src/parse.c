#include "lfparse.h"
#include <string.h>

static int valid(LfStr s) { return s.p || !s.n; }
static void advance(LfStr *s, size_t n) {
    if (n) s->p += n;
    s->n -= n;
}
static int digit(unsigned char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

static LfParseStatus magnitude(LfStr s, unsigned base, uint64_t limit,
                                size_t *used, uint64_t *value) {
    size_t n = 0;
    uint64_t v = 0;
    while (n < s.n) {
        int d = digit((unsigned char)s.p[n]);
        if (d < 0 || (unsigned)d >= base) break;
        if (v > limit / base || (v == limit / base && (unsigned)d > limit % base))
            return LF_PARSE_OVERFLOW;
        v = v * base + (unsigned)d;
        n++;
    }
    if (!n) return LF_PARSE_INVALID;
    *used = n;
    *value = v;
    return LF_PARSE_OK;
}

LfParseStatus lf_parse_u64(LfStr *input, unsigned base, uint64_t *value) {
    if (!input || !value || !valid(*input) || base < 2 || base > 36) return LF_PARSE_INVALID;
    LfStr s = *input;
    if (s.n && s.p[0] == '+') advance(&s, 1);
    uint64_t v;
    size_t n;
    LfParseStatus status = magnitude(s, base, UINT64_MAX, &n, &v);
    if (status != LF_PARSE_OK) return status;
    advance(&s, n);
    *input = s;
    *value = v;
    return LF_PARSE_OK;
}

LfParseStatus lf_parse_i64(LfStr *input, unsigned base, int64_t *value) {
    if (!input || !value || !valid(*input) || base < 2 || base > 36) return LF_PARSE_INVALID;
    LfStr s = *input;
    int negative = s.n && s.p[0] == '-';
    if (s.n && (s.p[0] == '+' || negative)) advance(&s, 1);
    uint64_t v, limit = (uint64_t)INT64_MAX + (unsigned)negative;
    size_t n;
    LfParseStatus status = magnitude(s, base, limit, &n, &v);
    if (status != LF_PARSE_OK) return status;
    int64_t result = negative ? (v == (uint64_t)INT64_MAX + 1 ? INT64_MIN : -(int64_t)v) : (int64_t)v;
    advance(&s, n);
    *input = s;
    *value = result;
    return LF_PARSE_OK;
}

LfParseStatus lf_parse_consume(LfStr *input, LfStr delimiter) {
    if (!input || !valid(*input) || !valid(delimiter) || delimiter.n > input->n)
        return LF_PARSE_INVALID;
    if (delimiter.n && memcmp(input->p, delimiter.p, delimiter.n)) return LF_PARSE_INVALID;
    advance(input, delimiter.n);
    return LF_PARSE_OK;
}

size_t lf_parse_skip_whitespace(LfStr *input) {
    if (!input || !valid(*input)) return 0;
    size_t n = 0;
    while (n < input->n) {
        char c = input->p[n];
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != '\f' && c != '\v') break;
        n++;
    }
    advance(input, n);
    return n;
}

LfParseStatus lf_parse_token(LfStr *input, LfStr delimiters, LfStr *token) {
    if (!input || !token || input == token || !valid(*input) || !valid(delimiters))
        return LF_PARSE_INVALID;
    size_t n = 0;
    while (n < input->n) {
        if (delimiters.n && memchr(delimiters.p, (unsigned char)input->p[n], delimiters.n)) break;
        n++;
    }
    if (!n) return LF_PARSE_INVALID;
    *token = (LfStr){input->p, n};
    advance(input, n);
    return LF_PARSE_OK;
}
