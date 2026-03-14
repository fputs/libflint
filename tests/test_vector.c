#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfvector.h"

int main() {
    Vector *v = malloc(sizeof(Vector));
    vec_init(v, NULL);

    int e0 = 0;
    int e1 = 1;
    int e2 = 2;
    int e3 = 3;
    int e4 = 4;

    vec_push(v, &e0);
    ASSERT_EQ(v->length, 1);
    int *t = vec_at(v, 0);
    ASSERT_EQ(*t, 0);

    vec_push(v, &e1);
    vec_push(v, &e2);
    ASSERT_EQ(v->length, 3);

    t = (int *) vec_safe_at(v, 3);
    ASSERT_NULL(t);

    vec_push(v, &e3);
    vec_insert(v, &e4, 1);

    t = (int *) vec_at(v, 4);
    ASSERT_EQ(*t, e3);
    t = (int *) vec_at(v, 1);
    ASSERT_EQ(*t, e4);

    const int *min = vec_min(v, vec_cmp_int);
    const int *max = vec_max(v, vec_cmp_int);
    ASSERT_EQ(*min, e0);
    ASSERT_EQ(*max, e4);

    t = (int *) vec_remove(v, 1);
    ASSERT_NOT_NULL(t);
    ASSERT_EQ(*t, 4);

    t = (int *) vec_remove(v, 10);
    ASSERT_NULL(t);

    vec_shrink(v);
    ASSERT_EQ(vec_len(v), vec_cap(v));

    vec_clear(v);
    ASSERT_EQ(vec_len(v), 0);

    vec_grow_to(v, 10);
    ASSERT_EQ(vec_cap(v), 10);
    ASSERT_EQ(vec_len(v), 0);

    vec_destroy(v);

    /* vec_pop */
    vec_init(v, NULL);
    int p1 = 10, p2 = 20, p3 = 30;
    vec_push(v, &p1);
    vec_push(v, &p2);
    vec_push(v, &p3);

    int *popped = vec_pop(v);
    ASSERT_EQ(*popped, 30);
    ASSERT_EQ(vec_len(v), 2);

    popped = vec_pop(v);
    ASSERT_EQ(*popped, 20);

    popped = vec_pop(v);
    ASSERT_EQ(*popped, 10);
    ASSERT_EQ(vec_len(v), 0);

    popped = vec_pop(v);
    ASSERT_NULL(popped);

    vec_destroy(v);

    /* vec_reverse */
    vec_init(v, NULL);
    int r1 = 1, r2 = 2, r3 = 3, r4 = 4;
    vec_push(v, &r1);
    vec_push(v, &r2);
    vec_push(v, &r3);
    vec_push(v, &r4);

    vec_reverse(v);
    ASSERT_EQ(*(int *)vec_at(v, 0), 4);
    ASSERT_EQ(*(int *)vec_at(v, 1), 3);
    ASSERT_EQ(*(int *)vec_at(v, 2), 2);
    ASSERT_EQ(*(int *)vec_at(v, 3), 1);

    /* reverse odd-length */
    vec_pop(v);
    vec_reverse(v);
    ASSERT_EQ(*(int *)vec_at(v, 0), 2);
    ASSERT_EQ(*(int *)vec_at(v, 1), 3);
    ASSERT_EQ(*(int *)vec_at(v, 2), 4);

    vec_destroy(v);

    /* vec_sort */
    vec_init(v, NULL);
    int s1 = 5, s2 = 1, s3 = 3, s4 = 2, s5 = 4;
    vec_push(v, &s1);
    vec_push(v, &s2);
    vec_push(v, &s3);
    vec_push(v, &s4);
    vec_push(v, &s5);

    vec_sort(v, vec_cmp_int);
    ASSERT_EQ(*(int *)vec_at(v, 0), 1);
    ASSERT_EQ(*(int *)vec_at(v, 1), 2);
    ASSERT_EQ(*(int *)vec_at(v, 2), 3);
    ASSERT_EQ(*(int *)vec_at(v, 3), 4);
    ASSERT_EQ(*(int *)vec_at(v, 4), 5);

    /* vec_bsearch on sorted vector */
    int key = 3;
    int *found = vec_bsearch(v, &key, vec_cmp_int);
    ASSERT_NOT_NULL(found);
    ASSERT_EQ(*found, 3);

    int missing = 99;
    found = vec_bsearch(v, &missing, vec_cmp_int);
    ASSERT_NULL(found);

    vec_destroy(v);
    free(v);

    TEST_REPORT();
}
