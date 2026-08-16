#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfhashset.h"

static int int_match(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b ? 0 : 1;
}

static size_t int_hash(const void *p) {
    return (size_t)*(const int *)p;
}

static int destroy_count;

static void counting_destroy(void *data) {
    free(data);
    destroy_count++;
}

int main(void) {
    ASSERT_EQ(lf_hashset_init(NULL, 8, int_match, int_hash, NULL), -1);
    LfHashSet bad;
    ASSERT_EQ(lf_hashset_init(&bad, 8, NULL, int_hash, NULL), -1);
    ASSERT_EQ(lf_hashset_init(&bad, 8, int_match, NULL, NULL), -1);

    LfHashSet set;
    ASSERT_EQ(lf_hashset_init(&set, 0, int_match, int_hash, NULL), 0);
    ASSERT_EQ(set.cap, (size_t)8);

    int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7, h = 8, i = 9;
    ASSERT_EQ(lf_hashset_insert(&set, &a), 0);
    ASSERT_EQ(lf_hashset_insert(&set, &b), 0);
    ASSERT_EQ(lf_hashset_insert(&set, &a), 1);
    ASSERT_EQ(lf_hashset_size(&set), 2);
    ASSERT_TRUE(lf_hashset_is_member(&set, &a));
    ASSERT_TRUE(lf_hashset_is_member(&set, &b));
    ASSERT_FALSE(lf_hashset_is_member(&set, &c));

    void *rm = &b;
    ASSERT_EQ(lf_hashset_remove(&set, &rm), 0);
    ASSERT_EQ(*(int *)rm, 2);
    ASSERT_FALSE(lf_hashset_is_member(&set, &b));
    ASSERT_EQ(lf_hashset_size(&set), 1);

    /* tombstone reuse */
    ASSERT_EQ(lf_hashset_insert(&set, &b), 0);
    ASSERT_TRUE(lf_hashset_is_member(&set, &b));

    rm = &c;
    ASSERT_EQ(lf_hashset_remove(&set, &rm), -1);
    ASSERT_EQ(lf_hashset_remove(&set, NULL), -1);

    /* force a grow */
    ASSERT_EQ(lf_hashset_insert(&set, &c), 0);
    ASSERT_EQ(lf_hashset_insert(&set, &d), 0);
    ASSERT_EQ(lf_hashset_insert(&set, &e), 0);
    ASSERT_EQ(lf_hashset_insert(&set, &f), 0);
    ASSERT_EQ(lf_hashset_insert(&set, &g), 0);
    ASSERT_EQ(lf_hashset_insert(&set, &h), 0);
    ASSERT_EQ(lf_hashset_insert(&set, &i), 0);
    ASSERT_EQ(lf_hashset_size(&set), 9);
    ASSERT_TRUE(set.cap >= 16);
    ASSERT_TRUE(lf_hashset_is_member(&set, &i));
    ASSERT_TRUE(lf_hashset_is_member(&set, &a));

    lf_hashset_destroy(&set);
    ASSERT_EQ(set.cap, (size_t)0);

    destroy_count = 0;
    LfHashSet owned;
    ASSERT_EQ(lf_hashset_init(&owned, 8, int_match, int_hash, counting_destroy), 0);
    int *p = malloc(sizeof(int));
    int *q = malloc(sizeof(int));
    *p = 10;
    *q = 20;
    ASSERT_EQ(lf_hashset_insert(&owned, p), 0);
    ASSERT_EQ(lf_hashset_insert(&owned, q), 0);
    rm = p;
    ASSERT_EQ(lf_hashset_remove(&owned, &rm), 0);
    ASSERT_EQ(destroy_count, 0);
    free(rm);
    lf_hashset_destroy(&owned);
    ASSERT_EQ(destroy_count, 1);

    TEST_REPORT();
}
