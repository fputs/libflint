#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfset.h"

static int int_match(const void *a, const void *b) {
    return *((int *) a) == *((int *) b);
}

int main() {
    Set *set = malloc(sizeof(Set));
    set_init(set, int_match, NULL);

    int i = 1;
    int j = 2;
    int k = 2;

    set_insert(set, &i);
    set_insert(set, &j);
    set_insert(set, &k);

    ASSERT_EQ(set->size, 2);  /* duplicate should not be inserted */

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

    ASSERT_EQ(set_u->size, 3);  /* {1, 2, 4} */
    ASSERT_EQ(set_d->size, 1);  /* {2} */
    ASSERT_EQ(set_i->size, 1);  /* {1} */

    set_destroy(set);
    set_destroy(set2);
    set_destroy(set_u);
    set_destroy(set_i);
    set_destroy(set_d);
    free(set);
    free(set2);
    free(set_u);
    free(set_i);
    free(set_d);

    TEST_REPORT();
}
