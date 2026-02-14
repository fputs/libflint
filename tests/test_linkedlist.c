#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lflinkedlist.h"

int main() {
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

    TEST_REPORT();
}
