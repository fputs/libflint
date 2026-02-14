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
    free(v);

    TEST_REPORT();
}
