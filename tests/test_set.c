#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfset.h"

static int int_match(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b ? 0 : 1;
}

static int other_match(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b ? 0 : 1;
}

static void free_data(void *data) {
    free(data);
}

int main() {
    ASSERT_EQ(set_init(NULL, int_match, NULL), -1);
    Set uninit;
    ASSERT_EQ(set_init(&uninit, NULL, NULL), -1);

    Set set;
    ASSERT_EQ(set_init(&set, int_match, NULL), 0);

    int i = 1;
    int j = 2;
    int k = 2;

    ASSERT_EQ(set_insert(&set, &i), 0);
    ASSERT_EQ(set_insert(&set, &j), 0);
    ASSERT_EQ(set_insert(&set, &k), 1);  /* duplicate returns 1 */

    ASSERT_EQ(set_size(&set), 2);

    ASSERT_TRUE(set_is_member(&set, &i));
    ASSERT_TRUE(set_is_member(&set, &j));
    int missing = 99;
    ASSERT_FALSE(set_is_member(&set, &missing));

    void *removed = &j;
    ASSERT_EQ(set_remove(&set, &removed), 0);
    ASSERT_EQ(set_size(&set), 1);
    ASSERT_FALSE(set_is_member(&set, &j));

    removed = &missing;
    ASSERT_EQ(set_remove(&set, &removed), -1);

    ASSERT_EQ(set_remove(&set, NULL), -1);

    set_insert(&set, &j);

    int i2 = 1;
    int j2 = 4;

    Set set2;
    ASSERT_EQ(set_init(&set2, int_match, NULL), 0);
    set_insert(&set2, &i2);
    set_insert(&set2, &j2);

    Set set_u, set_i, set_d;
    ASSERT_EQ(set_init(&set_u, int_match, NULL), 0);
    ASSERT_EQ(set_init(&set_i, int_match, NULL), 0);
    ASSERT_EQ(set_init(&set_d, int_match, NULL), 0);

    ASSERT_EQ(set_union(&set_u, &set, &set2), 0);
    ASSERT_EQ(set_difference(&set_d, &set, &set2), 0);
    ASSERT_EQ(set_intersection(&set_i, &set, &set2), 0);

    ASSERT_EQ(set_size(&set_u), 3);  /* {1, 2, 4} */
    ASSERT_EQ(set_size(&set_d), 1);  /* {2} */
    ASSERT_EQ(set_size(&set_i), 1);  /* {1} */

    Set sub;
    ASSERT_EQ(set_init(&sub, int_match, NULL), 0);
    set_insert(&sub, &i);  /* {1} is subset of {1, 2} */
    ASSERT_TRUE(set_is_subset(&sub, &set));
    ASSERT_FALSE(set_is_subset(&set, &sub));

    Set eq;
    ASSERT_EQ(set_init(&eq, int_match, NULL), 0);
    set_insert(&eq, &i);
    set_insert(&eq, &j);
    ASSERT_TRUE(set_is_equal(&set, &eq));
    ASSERT_FALSE(set_is_equal(&set, &sub));

    /* dest == a / dest == b rejected; dest left empty */
    ASSERT_EQ(set_union(&set, &set, &set2), -1);
    ASSERT_EQ(set_size(&set), 2);
    ASSERT_EQ(set_union(&set2, &set, &set2), -1);
    ASSERT_EQ(set_size(&set2), 2);

    /* dest already populated */
    ASSERT_EQ(set_union(&set_u, &set, &set2), -1);
    ASSERT_EQ(set_size(&set_u), 3);

    /* match mismatch */
    Set mismatch;
    ASSERT_EQ(set_init(&mismatch, other_match, NULL), 0);
    ASSERT_EQ(set_union(&mismatch, &set, &set2), -1);
    ASSERT_EQ(set_size(&mismatch), 0);

    /* dest with destroy is not a borrowed view */
    Set owned;
    ASSERT_EQ(set_init(&owned, int_match, free_data), 0);
    ASSERT_EQ(set_union(&owned, &set, &set2), -1);

    /* borrowed dest does not free source payloads */
    int *ha = malloc(sizeof(int)); *ha = 10;
    int *hb = malloc(sizeof(int)); *hb = 20;
    Set src;
    ASSERT_EQ(set_init(&src, int_match, free_data), 0);
    ASSERT_EQ(set_insert(&src, ha), 0);
    ASSERT_EQ(set_insert(&src, hb), 0);
    Set view;
    ASSERT_EQ(set_init(&view, int_match, NULL), 0);
    ASSERT_EQ(set_union(&view, &src, &src), 0);
    ASSERT_EQ(set_size(&view), 2);
    set_destroy(&view);
    ASSERT_EQ(*ha, 10);
    ASSERT_EQ(*hb, 20);
    set_destroy(&src);

    set_destroy(&set);
    set_destroy(&set2);
    set_destroy(&set_u);
    set_destroy(&set_i);
    set_destroy(&set_d);
    set_destroy(&sub);
    set_destroy(&eq);
    set_destroy(&mismatch);
    set_destroy(&owned);

    /* Heap-allocated data with destroy callback */
    Set hset;
    ASSERT_EQ(set_init(&hset, int_match, free_data), 0);

    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 10;  /* duplicate value */

    ASSERT_EQ(set_insert(&hset, a), 0);
    ASSERT_EQ(set_insert(&hset, b), 0);
    ASSERT_EQ(set_insert(&hset, c), 1);  /* duplicate */
    free(c);  /* caller responsible for rejected data */

    ASSERT_EQ(set_size(&hset), 2);
    ASSERT_TRUE(set_is_member(&hset, a));
    ASSERT_TRUE(set_is_member(&hset, b));

    set_destroy(&hset);

    TEST_REPORT();
}
