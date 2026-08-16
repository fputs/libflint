#ifndef LIBFLINT_LL_H
#define LIBFLINT_LL_H

#include <stddef.h>

typedef struct ListNode {
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

/*
 * Caller-owned handle. size, head, and tail are read-only after
 * ll_init; mutate the list through the insert/remove helpers.
 */
typedef struct {
    size_t size;

    void (*destroy)(void *data);

    int (*match)(const void *a, const void *b);

    struct ListNode *head;
    struct ListNode *tail;
} List;

void ll_init(List *list, void (*destroy)(void *data),
             int (*match)(const void *a, const void *b));

void ll_destroy(List *list);

int ll_ins_next(List *list, ListNode *node, const void *data);

int ll_ins_prev(List *list, ListNode *node, const void *data);

int ll_push_head(List *list, const void *data);

int ll_push_tail(List *list, const void *data);

int ll_pop_head(List *list, void **data);

int ll_pop_tail(List *list, void **data);

int ll_remove(List *list, ListNode *node, void **data);

int ll_remove_next(List *list, ListNode *node, void **data);

int ll_remove_prev(List *list, ListNode *node, void **data);

ListNode *ll_find(const List *list, const void *data);

void ll_reverse(List *list);

void ll_sort(List *list, int (*cmp)(const void *a, const void *b));

void **ll_to_array(const List *list);

#define LL_FOREACH(list, n) \
    for (ListNode *n = (list)->head; (n) != NULL; (n) = (n)->next)

#define LL_FOREACH_REV(list, n) \
    for (ListNode *n = (list)->tail; (n) != NULL; (n) = (n)->prev)

/* Compatibility aliases; the cursor is always named node. */
#define LL_ITER(list) LL_FOREACH(list, node)
#define LL_ITER_REV(list) LL_FOREACH_REV(list, node)

#endif
