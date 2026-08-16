#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfbst.h"

static int int_cmp(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    if (ia < ib) {
        return -1;
    }
    if (ia > ib) {
        return 1;
    }
    return 0;
}

static int destroy_count;
static int walk_buf[16];
static int walk_n;

static void counting_destroy(void *data) {
    free(data);
    destroy_count++;
}

static void collect_int(void *data) {
    walk_buf[walk_n++] = *(int *)data;
}

int main(void) {
    ASSERT_EQ(bst_init(NULL, int_cmp, NULL), -1);
    Bst bad;
    ASSERT_EQ(bst_init(&bad, NULL, NULL), -1);

    Bst t;
    ASSERT_EQ(bst_init(&t, int_cmp, NULL), 0);
    ASSERT_EQ(bst_size(&t), 0);
    ASSERT_NULL(bst_find(&t, &(int){1}));
    ASSERT_NULL(bst_min(&t));
    ASSERT_NULL(bst_max(&t));

    int vals[] = {8, 3, 10, 1, 6, 14, 4, 7};
    for (size_t i = 0; i < sizeof vals / sizeof vals[0]; i++) {
        ASSERT_EQ(bst_insert(&t, &vals[i]), 0);
    }
    ASSERT_EQ(bst_insert(&t, &vals[0]), 1);
    ASSERT_EQ(bst_size(&t), 8);

    ASSERT_EQ(*(int *)bst_find(&t, &(int){6}), 6);
    ASSERT_NULL(bst_find(&t, &(int){99}));
    ASSERT_EQ(*(int *)bst_min(&t), 1);
    ASSERT_EQ(*(int *)bst_max(&t), 14);

    walk_n = 0;
    bintree_traverse(&t.tree, bst_root(&t), BINTREE_INORDER, collect_int);
    ASSERT_EQ(walk_n, 8);
    int expect[] = {1, 3, 4, 6, 7, 8, 10, 14};
    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(walk_buf[i], expect[i]);
    }

    void *rm = &(int){1};
    ASSERT_EQ(bst_remove(&t, &rm), 0);
    ASSERT_EQ(*(int *)rm, 1);
    ASSERT_NULL(bst_find(&t, &(int){1}));
    ASSERT_EQ(*(int *)bst_min(&t), 3);
    ASSERT_EQ(bst_size(&t), 7);

    rm = &(int){8};
    ASSERT_EQ(bst_remove(&t, &rm), 0);
    ASSERT_EQ(*(int *)rm, 8);
    ASSERT_NULL(bst_find(&t, &(int){8}));
    ASSERT_EQ(*(int *)bst_root(&t)->data, 10);
    ASSERT_EQ(bst_size(&t), 6);

    walk_n = 0;
    bintree_traverse(&t.tree, bst_root(&t), BINTREE_INORDER, collect_int);
    ASSERT_EQ(walk_n, 6);
    int expect2[] = {3, 4, 6, 7, 10, 14};
    for (int i = 0; i < 6; i++) {
        ASSERT_EQ(walk_buf[i], expect2[i]);
    }

    rm = &(int){99};
    ASSERT_EQ(bst_remove(&t, &rm), -1);
    ASSERT_EQ(bst_remove(&t, NULL), -1);

    bst_destroy(&t);
    ASSERT_EQ(bst_size(&t), 0);

    destroy_count = 0;
    Bst owned;
    ASSERT_EQ(bst_init(&owned, int_cmp, counting_destroy), 0);
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 2;
    *b = 1;
    ASSERT_EQ(bst_insert(&owned, a), 0);
    ASSERT_EQ(bst_insert(&owned, b), 0);
    rm = &(int){2};
    ASSERT_EQ(bst_remove(&owned, &rm), 0);
    ASSERT_EQ(destroy_count, 0);
    free(rm);
    bst_destroy(&owned);
    ASSERT_EQ(destroy_count, 1);

    TEST_REPORT();
}
