#ifndef LIBFLINT_LL_H
#define LIBFLINT_LL_H

#include <stddef.h>

struct ListNode {
    void* data;
    struct ListNode* next;
    struct ListNode* prev;
};

struct List {
    size_t size;

    void (*destroy)(void* data);
    int (*match)(const void* a, const void* b);

    struct ListNode* head;
    struct ListNode* tail;
};

void ll_init(struct List* list, void (*destroy)(void *data));
void ll_destroy(struct List* list);
int ll_ins_next(struct List* list, struct ListNode* node, const void* data);
int ll_ins_prev(struct List* list, struct ListNode* node, const void* data);
int ll_remove(struct List* list, struct ListNode* node, void** data);
int ll_remove_next(struct List* list, struct ListNode* node, void** data);
int ll_remove_prev(struct List* list, struct ListNode* node, void** data);

#endif
