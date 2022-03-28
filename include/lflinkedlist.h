#ifndef LIBFLINT_LL_H
#define LIBFLINT_LL_H

#include <stddef.h>

typedef struct ListNode {
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

typedef struct {
    size_t size;

    void (*destroy)(void *data);

    int (*match)(const void *a, const void *b);

    struct ListNode *head;
    struct ListNode *tail;
} List;

void ll_init(List *list, void (*destroy)(void *data));

void ll_destroy(List *list);

int ll_ins_next(List *list, ListNode *node, const void *data);

int ll_ins_prev(List *list, ListNode *node, const void *data);

int ll_remove(List *list, ListNode *node, void **data);

int ll_remove_next(List *list, ListNode *node, void **data);

int ll_remove_prev(List *list, ListNode *node, void **data);

/* Provides ListNode *node for the iteration loop */
#define LL_ITER(list) for(ListNode *node = (list)->head; node != NULL; node = node->next)

#endif
