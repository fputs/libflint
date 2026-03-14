#include <string.h>
#include <stdlib.h>

#include "lflinkedlist.h"

void ll_init(List *list, void (*destroy)(void *data),
             int (*match)(const void *a, const void *b)) {
    list->size = 0;
    list->destroy = destroy;
    list->match = match;
    list->head = NULL;
    list->tail = NULL;
}

void ll_destroy(List *list) {
    void *data;
    while (list->size > 0) {
        if (ll_remove(list, list->tail, (void **) &data) == 0 && list->destroy != NULL) {
            list->destroy(data);
        }
    }
    memset(list, 0, sizeof(List));
}

int ll_ins_next(List *list, ListNode *node, const void *data) {
    ListNode *new_node;
    if (node == NULL && list->size != 0) {
        return -1;
    }
    if ((new_node = malloc(sizeof(ListNode))) == NULL) {
        return -1;
    }

    new_node->data = (void *) data;
    if (list->size == 0) {
        list->head = new_node;
        list->head->prev = NULL;
        list->head->next = NULL;
        list->tail = new_node;
    } else {
        new_node->next = node->next;
        new_node->prev = node;
        if (node->next == NULL) {
            list->tail = new_node;
        } else {
            node->next->prev = new_node;
        }
        node->next = new_node;
    }

    list->size++;
    return 0;
}

int ll_ins_prev(List *list, ListNode *node, const void *data) {
    ListNode *new_node;
    if (node == NULL && list->size != 0) {
        return -1;
    }
    if ((new_node = malloc(sizeof(ListNode))) == NULL) {
        return -1;
    }

    new_node->data = (void *) data;
    if (list->size == 0) {
        list->head = new_node;
        list->head->prev = NULL;
        list->head->next = NULL;
        list->tail = new_node;
    } else {
        new_node->next = node;
        new_node->prev = node->prev;
        if (node->prev == NULL) {
            list->head = new_node;
        } else {
            node->prev->next = new_node;
        }
        node->prev = new_node;
    }

    list->size++;
    return 0;
}

int ll_remove(List *list, ListNode *node, void **data) {
    if (node == NULL || data == NULL || list->size == 0) {
        return -1;
    }

    *data = node->data;
    if (node == list->head) {
        list->head = node->next;
        if (list->head == NULL) {
            list->tail = NULL;
        } else {
            node->next->prev = NULL;
        }
    } else {
        node->prev->next = node->next;
        if (node->next == NULL) {
            list->tail = node->prev;
        } else {
            node->next->prev = node->prev;
        }
    }
    free(node);
    list->size--;
    return 0;
}

int ll_remove_next(List *list, ListNode *node, void **data) {
    if (node == NULL || node->next == NULL) {
        return -1;
    }
    return ll_remove(list, node->next, data);
}

int ll_remove_prev(List *list, ListNode *node, void **data) {
    if (node == NULL || node->prev == NULL) {
        return -1;
    }
    return ll_remove(list, node->prev, data);
}

ListNode *ll_find(const List *list, const void *data) {
    if (list == NULL) {
        return NULL;
    }
    ListNode *node = list->head;
    while (node != NULL) {
        if (list->match != NULL) {
            if (list->match(node->data, data) == 0) {
                return node;
            }
        } else {
            if (node->data == data) {
                return node;
            }
        }
        node = node->next;
    }
    return NULL;
}

void ll_reverse(List *list) {
    ListNode *cur = list->head;
    ListNode *tmp;
    while (cur != NULL) {
        tmp = cur->next;
        cur->next = cur->prev;
        cur->prev = tmp;
        cur = tmp;
    }
    tmp = list->head;
    list->head = list->tail;
    list->tail = tmp;
}

static ListNode *ll_merge(ListNode *a, ListNode *b,
                           int (*cmp)(const void *, const void *)) {
    ListNode dummy;
    ListNode *tail = &dummy;
    dummy.next = NULL;

    while (a != NULL && b != NULL) {
        if (cmp(a->data, b->data) <= 0) {
            tail->next = a;
            a->prev = tail;
            a = a->next;
        } else {
            tail->next = b;
            b->prev = tail;
            b = b->next;
        }
        tail = tail->next;
    }

    tail->next = (a != NULL) ? a : b;
    if (tail->next != NULL) {
        tail->next->prev = tail;
    }

    dummy.next->prev = NULL;
    return dummy.next;
}

static ListNode *ll_mergesort(ListNode *head,
                               int (*cmp)(const void *, const void *)) {
    if (head == NULL || head->next == NULL) {
        return head;
    }

    /* split in half with slow/fast pointers */
    ListNode *slow = head;
    ListNode *fast = head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode *second = slow->next;
    slow->next = NULL;
    if (second != NULL) {
        second->prev = NULL;
    }

    ListNode *a = ll_mergesort(head, cmp);
    ListNode *b = ll_mergesort(second, cmp);
    return ll_merge(a, b, cmp);
}

void ll_sort(List *list, int (*cmp)(const void *a, const void *b)) {
    if (list->size < 2) {
        return;
    }
    list->head = ll_mergesort(list->head, cmp);

    /* fix tail pointer */
    ListNode *node = list->head;
    while (node->next != NULL) {
        node = node->next;
    }
    list->tail = node;
}

void **ll_to_array(const List *list) {
    if (list == NULL || list->size == 0) {
        return NULL;
    }
    void **arr = malloc(list->size * sizeof(void *));
    if (arr == NULL) {
        return NULL;
    }
    size_t i = 0;
    for (ListNode *node = list->head; node != NULL; node = node->next) {
        arr[i++] = node->data;
    }
    return arr;
}

