#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfset.h"

static int int_match(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

static void free_data(void *data) {
    free(data);
}

int main() {
    /* Basic insert and duplicate rejection */
    Set *set = malloc(sizeof(Set));
    set_init(set, int_match, NULL);

    int i = 1;
    int j = 2;
    int k = 2;

    ASSERT_EQ(set_insert(set, &i), 0);
    ASSERT_EQ(set_insert(set, &j), 0);
    ASSERT_EQ(set_insert(set, &k), 1);  /* duplicate returns 1 */

    ASSERT_EQ(set_size(set), 2);

    /* set_is_member */
    ASSERT_TRUE(set_is_member(set, &i));
    ASSERT_TRUE(set_is_member(set, &j));
    int missing = 99;
    ASSERT_FALSE(set_is_member(set, &missing));

    /* set_remove */
    void *removed = &j;
    ASSERT_EQ(set_remove(set, &removed), 0);
    ASSERT_EQ(set_size(set), 1);
    ASSERT_FALSE(set_is_member(set, &j));

    /* set_remove: not found */
    removed = &missing;
    ASSERT_EQ(set_remove(set, &removed), -1);

    /* set_remove: NULL data pointer */
    ASSERT_EQ(set_remove(set, NULL), -1);

    /* Re-insert removed element for set operations */
    set_insert(set, &j);

    int i2 = 1;
    int j2 = 4;

    Set *set2 = malloc(sizeof(Set));
    set_init(set2, int_match, NULL);
    set_insert(set2, &i2);
    set_insert(set2, &j2);

    Set *set_u = malloc(sizeof(Set));
    Set *set_i = malloc(sizeof(Set));
    Set *set_d = malloc(sizeof(Set));

    set_union(set_u, set, set2);
    set_difference(set_d, set, set2);
    set_intersection(set_i, set, set2);

    ASSERT_EQ(set_size(set_u), 3);  /* {1, 2, 4} */
    ASSERT_EQ(set_size(set_d), 1);  /* {2} */
    ASSERT_EQ(set_size(set_i), 1);  /* {1} */

    /* set_is_subset */
    Set *sub = malloc(sizeof(Set));
    set_init(sub, int_match, NULL);
    set_insert(sub, &i);  /* {1} is subset of {1, 2} */
    ASSERT_TRUE(set_is_subset(sub, set));
    ASSERT_FALSE(set_is_subset(set, sub));

    /* set_is_equal */
    Set *eq = malloc(sizeof(Set));
    set_init(eq, int_match, NULL);
    set_insert(eq, &i);
    set_insert(eq, &j);
    ASSERT_TRUE(set_is_equal(set, eq));
    ASSERT_FALSE(set_is_equal(set, sub));

    set_destroy(set);
    set_destroy(set2);
    set_destroy(set_u);
    set_destroy(set_i);
    set_destroy(set_d);
    set_destroy(sub);
    set_destroy(eq);
    free(set);
    free(set2);
    free(set_u);
    free(set_i);
    free(set_d);
    free(sub);
    free(eq);

    /* Heap-allocated data with destroy callback */
    Set *hset = malloc(sizeof(Set));
    set_init(hset, int_match, free_data);

    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 10;  /* duplicate value */

    ASSERT_EQ(set_insert(hset, a), 0);
    ASSERT_EQ(set_insert(hset, b), 0);
    ASSERT_EQ(set_insert(hset, c), 1);  /* duplicate */
    free(c);  /* caller responsible for rejected data */

    ASSERT_EQ(set_size(hset), 2);
    ASSERT_TRUE(set_is_member(hset, a));
    ASSERT_TRUE(set_is_member(hset, b));

    /* destroy frees heap data */
    set_destroy(hset);
    free(hset);

    TEST_REPORT();
}
