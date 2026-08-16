#ifndef LFTEST_H
#define LFTEST_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int lf_tests_total;
static int lf_tests_failed;

#define ASSERT_TRUE(expr) do { \
    lf_tests_total++; \
    if (!(expr)) { \
        lf_tests_failed++; \
        fprintf(stderr, "  FAIL %s:%d: %s\n", __FILE__, __LINE__, #expr); \
    } \
} while(0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

#define ASSERT_EQ(a, b) do { \
    lf_tests_total++; \
    if ((a) != (b)) { \
        lf_tests_failed++; \
        fprintf(stderr, "  FAIL %s:%d: %s != %s (%lld != %lld)\n", \
            __FILE__, __LINE__, #a, #b, \
            (long long)(intptr_t)(a), (long long)(intptr_t)(b)); \
    } \
} while(0)

#define ASSERT_NEQ(a, b) do { \
    lf_tests_total++; \
    if ((a) == (b)) { \
        lf_tests_failed++; \
        fprintf(stderr, "  FAIL %s:%d: %s == %s (%lld)\n", \
            __FILE__, __LINE__, #a, #b, (long long)(intptr_t)(a)); \
    } \
} while(0)

#define ASSERT_LT(a, b) do { \
    lf_tests_total++; \
    if (!((a) < (b))) { \
        lf_tests_failed++; \
        fprintf(stderr, "  FAIL %s:%d: %s >= %s (%lld >= %lld)\n", \
            __FILE__, __LINE__, #a, #b, \
            (long long)(intptr_t)(a), (long long)(intptr_t)(b)); \
    } \
} while(0)

#define ASSERT_STR_EQ(a, b) do { \
    lf_tests_total++; \
    const char *_lf_sa = (a); \
    const char *_lf_sb = (b); \
    if (_lf_sa == NULL || _lf_sb == NULL) { \
        if (_lf_sa != _lf_sb) { \
            lf_tests_failed++; \
            fprintf(stderr, "  FAIL %s:%d: %s != %s (%s vs %s)\n", \
                __FILE__, __LINE__, #a, #b, \
                _lf_sa ? _lf_sa : "NULL", _lf_sb ? _lf_sb : "NULL"); \
        } \
    } else if (strcmp(_lf_sa, _lf_sb) != 0) { \
        lf_tests_failed++; \
        fprintf(stderr, "  FAIL %s:%d: \"%s\" != \"%s\"\n", \
            __FILE__, __LINE__, _lf_sa, _lf_sb); \
    } \
} while(0)

#define ASSERT_NULL(expr) ASSERT_EQ((void *)(expr), NULL)
#define ASSERT_NOT_NULL(expr) ASSERT_NEQ((void *)(expr), NULL)

#define TEST_REPORT() do { \
    printf("%d/%d passed\n", lf_tests_total - lf_tests_failed, lf_tests_total); \
    return lf_tests_failed ? 1 : 0; \
} while(0)

#endif /* LFTEST_H */
