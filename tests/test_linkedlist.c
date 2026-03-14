#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lflinkedlist.h"

static int match_int(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

static int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

int main() {
    /* Basic insertion and size */
    List *list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

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

    /* ll_ins_prev */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

    int a = 10, b = 20, c = 30;
    ll_ins_next(list, list->head, &b);      /* [20] */
    ll_ins_prev(list, list->head, &a);      /* [10, 20] */
    ll_ins_next(list, list->tail, &c);      /* [10, 20, 30] */

    ASSERT_EQ(list->size, 3);
    ASSERT_EQ(*(int *)list->head->data, 10);
    ASSERT_EQ(*(int *)list->head->next->data, 20);
    ASSERT_EQ(*(int *)list->tail->data, 30);

    /* ll_ins_prev into middle */
    int d = 15;
    ll_ins_prev(list, list->head->next, &d); /* [10, 15, 20, 30] */
    ASSERT_EQ(list->size, 4);
    ASSERT_EQ(*(int *)list->head->next->data, 15);

    ll_destroy(list);
    free(list);

    /* ll_remove direct */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

    int x = 100, y = 200, z = 300;
    ll_ins_next(list, list->head, &x);
    ll_ins_next(list, list->tail, &y);
    ll_ins_next(list, list->tail, &z);

    /* Remove middle node */
    ll_remove(list, list->head->next, &data);
    ASSERT_EQ(*(int *)data, 200);
    ASSERT_EQ(list->size, 2);
    ASSERT_EQ(*(int *)list->head->data, 100);
    ASSERT_EQ(*(int *)list->tail->data, 300);

    /* Remove head */
    ll_remove(list, list->head, &data);
    ASSERT_EQ(*(int *)data, 100);
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(*(int *)list->head->data, 300);

    /* Remove tail (last element) */
    ll_remove(list, list->tail, &data);
    ASSERT_EQ(*(int *)data, 300);
    ASSERT_EQ(list->size, 0);
    ASSERT_NULL(list->head);
    ASSERT_NULL(list->tail);

    ll_destroy(list);
    free(list);

    /* ll_remove_prev */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

    int p = 1, q = 2, r = 3;
    ll_ins_next(list, list->head, &p);
    ll_ins_next(list, list->tail, &q);
    ll_ins_next(list, list->tail, &r);

    /* Remove prev of tail (should remove middle) */
    ll_remove_prev(list, list->tail, &data);
    ASSERT_EQ(*(int *)data, 2);
    ASSERT_EQ(list->size, 2);

    /* Remove prev of head should fail */
    ASSERT_EQ(ll_remove_prev(list, list->head, &data), -1);

    ll_destroy(list);
    free(list);

    /* ll_remove NULL data pointer */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

    int val = 42;
    ll_ins_next(list, list->head, &val);
    ASSERT_EQ(ll_remove(list, list->head, NULL), -1);
    ASSERT_EQ(list->size, 1);

    ll_destroy(list);
    free(list);

    /* Destroy callback with free (heap-allocated data) */
    list = malloc(sizeof(List));
    ll_init(list, free, NULL);

    for (int n = 0; n < 5; n++) {
        int *heap_val = malloc(sizeof(int));
        *heap_val = n * 10;
        ll_ins_next(list, list->tail, heap_val);
    }
    ASSERT_EQ(list->size, 5);
    ll_destroy(list);
    ASSERT_EQ(list->size, 0);
    free(list);

    /* Error paths: NULL node for insert when list non-empty */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

    int e = 1;
    ll_ins_next(list, list->head, &e);
    ASSERT_EQ(ll_ins_next(list, NULL, &e), -1);
    ASSERT_EQ(ll_ins_prev(list, NULL, &e), -1);

    /* Error paths: remove from empty list */
    List *empty = malloc(sizeof(List));
    ll_init(empty, NULL, NULL);
    ASSERT_EQ(ll_remove(empty, NULL, &data), -1);
    ASSERT_EQ(ll_remove_next(empty, NULL, &data), -1);
    ASSERT_EQ(ll_remove_prev(empty, NULL, &data), -1);
    free(empty);

    ll_destroy(list);
    free(list);

    /* ll_find with match callback */
    list = malloc(sizeof(List));
    ll_init(list, NULL, match_int);

    int f1 = 10, f2 = 20, f3 = 30;
    ll_ins_next(list, list->head, &f1);
    ll_ins_next(list, list->tail, &f2);
    ll_ins_next(list, list->tail, &f3);

    int search = 20;
    ListNode *found = ll_find(list, &search);
    ASSERT_NOT_NULL(found);
    ASSERT_EQ(*(int *)found->data, 20);

    int missing = 99;
    ASSERT_NULL(ll_find(list, &missing));

    /* ll_find with NULL match (pointer comparison) */
    list->match = NULL;
    found = ll_find(list, &f2);
    ASSERT_NOT_NULL(found);
    ASSERT_EQ(*(int *)found->data, 20);

    ASSERT_NULL(ll_find(list, &missing));
    ASSERT_NULL(ll_find(NULL, &search));

    ll_destroy(list);
    free(list);

    /* ll_reverse */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

    int ra = 10, rb = 20, rc = 30;
    ll_ins_next(list, list->head, &ra);
    ll_ins_next(list, list->tail, &rb);
    ll_ins_next(list, list->tail, &rc);

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

    ll_destroy(list);
    free(list);

    /* ll_sort */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

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
    ll_init(list, NULL, NULL);
    ll_sort(list, cmp_int);
    ASSERT_EQ(list->size, 0);

    int solo = 42;
    ll_ins_next(list, list->head, &solo);
    ll_sort(list, cmp_int);
    ASSERT_EQ(list->size, 1);
    ASSERT_EQ(*(int *)list->head->data, 42);

    ll_destroy(list);
    free(list);

    /* ll_to_array: single element */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

    int single = 42;
    ll_ins_next(list, list->head, &single);

    void **arr = ll_to_array(list);
    ASSERT_NOT_NULL(arr);
    ASSERT_EQ(*(int *)arr[0], 42);
    free(arr);

    ll_destroy(list);
    free(list);

    /* ll_to_array: empty list */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);
    ASSERT_NULL(ll_to_array(list));
    ASSERT_NULL(ll_to_array(NULL));
    ll_destroy(list);
    free(list);

    /* ll_to_array: multiple elements */
    list = malloc(sizeof(List));
    ll_init(list, NULL, NULL);

    int m1 = 1, m2 = 2, m3 = 3;
    ll_ins_next(list, list->head, &m1);
    ll_ins_next(list, list->tail, &m2);
    ll_ins_next(list, list->tail, &m3);

    arr = ll_to_array(list);
    ASSERT_NOT_NULL(arr);
    ASSERT_EQ(*(int *)arr[0], 1);
    ASSERT_EQ(*(int *)arr[1], 2);
    ASSERT_EQ(*(int *)arr[2], 3);
    free(arr);

    ll_destroy(list);
    free(list);

    TEST_REPORT();
}
