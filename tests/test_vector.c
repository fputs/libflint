#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfvector.h"

static int destroy_count;

static void counting_destroy(void *data) {
    (void)data;
    destroy_count++;
}

int main() {
    /* --- existing tests --- */
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

    /* --- destroy callback --- */
    destroy_count = 0;
    Vector dv;
    vec_init(&dv, counting_destroy);
    int a = 1, b = 2, c = 3;
    vec_push(&dv, &a);
    vec_push(&dv, &b);
    vec_push(&dv, &c);
    vec_destroy(&dv);
    ASSERT_EQ(destroy_count, 3);

    /* --- vec_init_with_capacity --- */
    Vector cv;
    ASSERT_EQ(vec_init_with_capacity(&cv, NULL, 16), 0);
    ASSERT_EQ(vec_cap(&cv), 16);
    ASSERT_EQ(vec_len(&cv), 0);
    vec_destroy(&cv);

    /* --- vec_init NULL check --- */
    ASSERT_EQ(vec_init(NULL, NULL), -1);
    ASSERT_EQ(vec_init_with_capacity(NULL, NULL, 4), -1);

    /* --- insert at index 0 --- */
    Vector iv;
    vec_init(&iv, NULL);
    int x1 = 10, x2 = 20, x3 = 30;
    vec_push(&iv, &x1);
    vec_push(&iv, &x2);
    vec_insert(&iv, &x3, 0);
    ASSERT_EQ(*(int *)vec_at(&iv, 0), 30);
    ASSERT_EQ(*(int *)vec_at(&iv, 1), 10);
    ASSERT_EQ(*(int *)vec_at(&iv, 2), 20);
    ASSERT_EQ(vec_len(&iv), 3);

    /* --- insert out of bounds --- */
    ASSERT_EQ(vec_insert(&iv, &x1, 100), -1);

    vec_destroy(&iv);

    /* --- vec_sort empty --- */
    Vector sv;
    vec_init(&sv, NULL);
    vec_sort(&sv, vec_cmp_int);
    ASSERT_EQ(vec_len(&sv), 0);

    /* --- vec_sort single element --- */
    int ss1 = 42;
    vec_push(&sv, &ss1);
    vec_sort(&sv, vec_cmp_int);
    ASSERT_EQ(vec_len(&sv), 1);
    ASSERT_EQ(*(int *)vec_at(&sv, 0), 42);

    /* --- vec_sort multiple elements --- */
    int ss2 = 5, ss3 = 99, ss4 = 1;
    vec_push(&sv, &ss2);
    vec_push(&sv, &ss3);
    vec_push(&sv, &ss4);
    vec_sort(&sv, vec_cmp_int);
    ASSERT_EQ(*(int *)vec_at(&sv, 0), 1);
    ASSERT_EQ(*(int *)vec_at(&sv, 1), 5);
    ASSERT_EQ(*(int *)vec_at(&sv, 2), 42);
    ASSERT_EQ(*(int *)vec_at(&sv, 3), 99);
    vec_destroy(&sv);

    /* --- vec_shrink on empty vector --- */
    Vector ev;
    vec_init(&ev, NULL);
    ASSERT_EQ(vec_shrink(&ev), 0);
    ASSERT_EQ(vec_cap(&ev), 0);
    ASSERT_NULL(ev.elements);

    /* --- vec_clear keeps capacity --- */
    Vector clv;
    vec_init(&clv, NULL);
    int cl1 = 1;
    vec_push(&clv, &cl1);
    size_t kept_cap = vec_cap(&clv);
    ASSERT_EQ(vec_clear(&clv), 0);
    ASSERT_EQ(vec_len(&clv), 0);
    ASSERT_EQ(vec_cap(&clv), kept_cap);
    vec_destroy(&clv);

    /* --- vec_remove out of bounds --- */
    Vector rv;
    vec_init(&rv, NULL);
    ASSERT_NULL(vec_remove(&rv, 0));
    ASSERT_NULL(vec_safe_at(&rv, 0));
    vec_destroy(&rv);

    /* --- vec_grow_to smaller fails --- */
    Vector gv;
    vec_init(&gv, NULL);
    ASSERT_EQ(vec_grow_to(&gv, 1), -1);
    ASSERT_EQ(vec_grow_to(&gv, vec_cap(&gv)), 0);
    vec_destroy(&gv);

    /* --- insert after grow preserves prior elements --- */
    Vector ig;
    vec_init(&ig, NULL);
    int g0 = 0, g1 = 1, g2 = 2, g3 = 3, gmid = 99;
    vec_push(&ig, &g0);
    vec_push(&ig, &g1);
    ASSERT_EQ(vec_len(&ig), vec_cap(&ig));
    ASSERT_EQ(vec_insert(&ig, &gmid, 1), 0);
    ASSERT_EQ(*(int *)vec_at(&ig, 0), 0);
    ASSERT_EQ(*(int *)vec_at(&ig, 1), 99);
    ASSERT_EQ(*(int *)vec_at(&ig, 2), 1);
    vec_push(&ig, &g2);
    vec_push(&ig, &g3);
    ASSERT_EQ(vec_len(&ig), 5);
    ASSERT_EQ(*(int *)vec_at(&ig, 3), 2);
    ASSERT_EQ(*(int *)vec_at(&ig, 4), 3);
    vec_destroy(&ig);

    /* --- push after shrink of empty --- */
    Vector se;
    vec_init(&se, NULL);
    ASSERT_EQ(vec_shrink(&se), 0);
    ASSERT_EQ(vec_cap(&se), 0);
    ASSERT_NULL(se.elements);
    int se1 = 7;
    ASSERT_EQ(vec_push(&se, &se1), 0);
    ASSERT_EQ(vec_len(&se), 1);
    ASSERT_EQ(*(int *)vec_at(&se, 0), 7);
    vec_destroy(&se);

    /* --- init_with_capacity(0) --- */
    Vector zc;
    ASSERT_EQ(vec_init_with_capacity(&zc, NULL, 0), 0);
    ASSERT_EQ(vec_cap(&zc), 0);
    ASSERT_EQ(vec_len(&zc), 0);
    ASSERT_NULL(zc.elements);
    int z1 = 11;
    ASSERT_EQ(vec_push(&zc, &z1), 0);
    ASSERT_EQ(*(int *)vec_at(&zc, 0), 11);
    vec_destroy(&zc);

    /* --- stored NULL payloads --- */
    Vector nv;
    vec_init(&nv, NULL);
    ASSERT_EQ(vec_push(&nv, NULL), 0);
    ASSERT_NULL(vec_at(&nv, 0));
    ASSERT_EQ(vec_len(&nv), 1);
    ASSERT_NULL(vec_pop(&nv));
    ASSERT_EQ(vec_len(&nv), 0);
    vec_destroy(&nv);

    /* --- vec_reserve no-op when already large enough --- */
    Vector rsv;
    vec_init_with_capacity(&rsv, NULL, 8);
    void **same = rsv.elements;
    ASSERT_EQ(vec_reserve(&rsv, 4), 0);
    ASSERT_EQ(vec_cap(&rsv), 8);
    ASSERT_EQ(rsv.elements, same);
    ASSERT_EQ(vec_reserve(&rsv, 16), 0);
    ASSERT_EQ(vec_cap(&rsv), 16);
    vec_destroy(&rsv);

    /* --- double destroy is safe --- */
    Vector dd;
    vec_init(&dd, NULL);
    vec_push(&dd, &e0);
    vec_destroy(&dd);
    vec_destroy(&dd);

    TEST_REPORT();
}
