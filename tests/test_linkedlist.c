#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lflinkedlist.h"

static int match_int(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b;
}

static int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

int main() {
    /* basic insert/remove */
    List *list = malloc(sizeof(List));
    ll_init(list, NULL);

    int i = 1;
    int j = 2;
    int k = 4;

    ll_ins_next(list, list->head, (void *) &i);
    ll_ins_next(list, list->tail, (void *) &j);
    ll_ins_next(list, list->tail, (void *) &k);

    ASSERT_EQ(list->size, 3);

    void *data;
    ll_remove_next(list, list->head, &data);
    ASSERT_EQ(*(int *)data, 2);
    ASSERT_EQ(list->size, 2);

    ll_destroy(list);
    free(list);

    /* ll_find */
    list = malloc(sizeof(List));
    ll_init(list, NULL);
    list->match = match_int;

    int a = 10, b = 20, c = 30;
    ll_ins_next(list, list->head, &a);
    ll_ins_next(list, list->tail, &b);
    ll_ins_next(list, list->tail, &c);

    ListNode *found = ll_find(list, &b);
    ASSERT_NOT_NULL(found);
    ASSERT_EQ(*(int *)found->data, 20);

    int missing = 99;
    found = ll_find(list, &missing);
    ASSERT_NULL(found);

    /* ll_find with no match callback */
    list->match = NULL;
    found = ll_find(list, &a);
    ASSERT_NULL(found);
    list->match = match_int;

    /* ll_reverse */
    ll_reverse(list);
    ASSERT_EQ(*(int *)list->head->data, 30);
    ASSERT_EQ(*(int *)list->tail->data, 10);

    int expected_rev[] = {30, 20, 10};
    int idx = 0;
    LL_ITER(list) {
        ASSERT_EQ(*(int *)node->data, expected_rev[idx++]);
    }

    /* LL_ITER_REV */
    int expected_fwd[] = {10, 20, 30};
    idx = 0;
    LL_ITER_REV(list) {
        ASSERT_EQ(*(int *)node->data, expected_fwd[idx++]);
    }

    /* ll_to_array */
    void **arr = ll_to_array(list);
    ASSERT_NOT_NULL(arr);
    ASSERT_EQ(*(int *)arr[0], 30);
    ASSERT_EQ(*(int *)arr[1], 20);
    ASSERT_EQ(*(int *)arr[2], 10);
    free(arr);

    ll_destroy(list);
    free(list);

    /* ll_sort */
    list = malloc(sizeof(List));
    ll_init(list, NULL);

    int s1 = 3, s2 = 1, s3 = 5, s4 = 2, s5 = 4;
    ll_ins_next(list, list->head, &s1);
    ll_ins_next(list, list->tail, &s2);
    ll_ins_next(list, list->tail, &s3);
    ll_ins_next(list, list->tail, &s4);
    ll_ins_next(list, list->tail, &s5);

    ll_sort(list, cmp_int);
    ASSERT_EQ(list->size, 5);
    ASSERT_EQ(*(int *)list->head->data, 1);
    ASSERT_EQ(*(int *)list->tail->data, 5);

    int expected_sorted[] = {1, 2, 3, 4, 5};
    idx = 0;
    LL_ITER(list) {
        ASSERT_EQ(*(int *)node->data, expected_sorted[idx++]);
    }

    /* verify prev pointers are correct after sort */
    idx = 4;
    LL_ITER_REV(list) {
        ASSERT_EQ(*(int *)node->data, expected_sorted[idx--]);
    }

    /* sort empty and single-element lists */
    ll_destroy(list);
    ll_init(list, NULL);
    ll_sort(list, cmp_int);
    ASSERT_EQ(list->size, 0);

    int solo = 42;
    ll_ins_next(list, list->head, &solo);
    ll_sort(list, cmp_int);
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(*(int *)list->head->data, 42);

    /* ll_to_array on empty list */
    ll_destroy(list);
    ll_init(list, NULL);
    arr = ll_to_array(list);
    ASSERT_NULL(arr);

    ll_destroy(list);
    free(list);

    TEST_REPORT();
}
