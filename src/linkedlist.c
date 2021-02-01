#include <string.h>
#include <stdlib.h>

#include "linkedlist.h"

void ll_init(struct List* list, void (*destroy)(void *data)) {
    list->size = 0;
    list->destroy = destroy;
    list->head = NULL;
    list->tail = NULL;
}

void ll_destroy(struct List* list) {
    void* data;
    while (list->size > 0) {
        if (ll_remove(list, list->tail, (void**)&data) == 0 && list->destroy != NULL) {
            list->destroy(data);
        }
    }
    memset(list, 0, sizeof(struct List));
}

int ll_ins_next(struct List* list, struct ListNode* node, const void* data) {
    struct ListNode* new_node;
    if (node == NULL && list->size != 0) {
        return -1;
    }
    if ((new_node = malloc(sizeof(struct ListNode))) == NULL) {
        return -1;
    }

    new_node->data = (void*)data;
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

int ll_ins_prev(struct List* list, struct ListNode* node, const void* data) {
    struct ListNode* new_node;
    if (node == NULL && list->size != 0) {
        return -1;
    }
    if ((new_node = malloc(sizeof(struct ListNode))) == NULL) {
        return -1;
    }

    new_node->data = (void*)data;
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

int ll_remove(struct List* list, struct ListNode* node, void** data) {
    if (node == NULL || list->size == 0) {
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

int ll_remove_next(struct List* list, struct ListNode* node, void** data) {
    if (node->next == NULL) {
        return -1;
    }
    return ll_remove(list, node->next, data);
}

int ll_remove_prev(struct List* list, struct ListNode* node, void** data) {
    if (node->prev == NULL) {
        return -1;
    }
    return ll_remove(list, node->prev, data);
}

