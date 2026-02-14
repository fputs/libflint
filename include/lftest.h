#ifndef LFTEST_H
#define LFTEST_H

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
        fprintf(stderr, "  FAIL %s:%d: %s != %s\n", __FILE__, __LINE__, #a, #b); \
    } \
} while(0)

#define ASSERT_NEQ(a, b) do { \
    lf_tests_total++; \
    if ((a) == (b)) { \
        lf_tests_failed++; \
        fprintf(stderr, "  FAIL %s:%d: %s == %s\n", __FILE__, __LINE__, #a, #b); \
    } \
} while(0)

#define ASSERT_LT(a, b) do { \
    lf_tests_total++; \
    if (!((a) < (b))) { \
        lf_tests_failed++; \
        fprintf(stderr, "  FAIL %s:%d: %s >= %s\n", __FILE__, __LINE__, #a, #b); \
    } \
} while(0)

#define ASSERT_STR_EQ(a, b) do { \
    lf_tests_total++; \
    if (strcmp((a), (b)) != 0) { \
        lf_tests_failed++; \
        fprintf(stderr, "  FAIL %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, (a), (b)); \
    } \
} while(0)

#define ASSERT_NULL(expr) ASSERT_EQ((void *)(expr), NULL)
#define ASSERT_NOT_NULL(expr) ASSERT_NEQ((void *)(expr), NULL)

#define TEST_REPORT() do { \
    printf("%d/%d passed\n", lf_tests_total - lf_tests_failed, lf_tests_total); \
    return lf_tests_failed ? 1 : 0; \
} while(0)

#endif /* LFTEST_H */
