#include "lfhashmap.h"
#include "lftest.h"
#include <stdlib.h>

static int match(const void *a, const void *b) { return *(const int *)a != *(const int *)b; }
static size_t hash(const void *a) { (void)a; return 7; }
static int keys, values;
static void dk(void *p) { keys++; free(p); }
static void dv(void *p) { values++; free(p); }
static int *number(int n) { int *p = malloc(sizeof(*p)); if (!p) abort(); *p = n; return p; }

int main(void) {
    LfHashMap m;
    ASSERT_EQ(lf_hashmap_init(&m, SIZE_MAX, match, hash, NULL, NULL), -1);
    lf_hashmap_destroy(&m);
    ASSERT_EQ(lf_hashmap_init(&m, 0, match, hash, NULL, NULL), 0);
    int k[200];
    for (int i = 0; i < 200; i++) {
        k[i] = i;
        ASSERT_EQ(lf_hashmap_insert(&m, &k[i], i ? &k[i] : NULL), 0);
    }
    void *v = &m, *key = NULL;
    ASSERT_EQ(lf_hashmap_find(&m, &k[0], &v), 0);
    ASSERT_NULL(v);
    ASSERT_EQ(lf_hashmap_insert(&m, &k[0], &m), 1);
    for (int i = 0; i < 200; i += 2) {
        ASSERT_EQ(lf_hashmap_remove(&m, &k[i], &key, &v), 0);
        ASSERT_EQ(key, &k[i]);
    }
    for (int i = 1; i < 200; i += 2) {
        ASSERT_EQ(lf_hashmap_find(&m, &k[i], &v), 0);
        ASSERT_EQ(v, &k[i]);
    }
    for (int i = 0; i < 200; i += 2) ASSERT_EQ(lf_hashmap_insert(&m, &k[i], NULL), 0);
    ASSERT_EQ(m.len, 200);
    lf_hashmap_clear(&m);
    v = &m;
    ASSERT_EQ(lf_hashmap_find(&m, &k[0], &v), 1);
    ASSERT_EQ(v, &m);
    ASSERT_EQ(lf_hashmap_replace(&m, &k[0], NULL), 1);
    lf_hashmap_destroy(&m);
    ASSERT_EQ(lf_hashmap_init(&m, 0, match, hash, dk, dv), 0);
    int *a = number(1), *b = number(2);
    ASSERT_EQ(lf_hashmap_insert(&m, a, b), 0);
    ASSERT_EQ(lf_hashmap_replace(&m, a, b), 0);
    ASSERT_EQ(keys + values, 0);
    a = number(1); b = number(3);
    ASSERT_EQ(lf_hashmap_replace(&m, a, b), 0);
    ASSERT_EQ(keys, 1); ASSERT_EQ(values, 1);
    ASSERT_EQ(lf_hashmap_remove(&m, a, &key, &v), 0);
    ASSERT_EQ(keys + values, 2);
    free(key); free(v);
    ASSERT_EQ(lf_hashmap_insert(&m, number(4), NULL), 0);
    lf_hashmap_clear(&m);
    ASSERT_EQ(keys, 2); ASSERT_EQ(values, 2);
    ASSERT_EQ(lf_hashmap_insert(&m, number(5), number(6)), 0);
    lf_hashmap_destroy(&m);
    ASSERT_EQ(keys, 3); ASSERT_EQ(values, 3);
    lf_hashmap_destroy(&m);
    TEST_REPORT();
}
