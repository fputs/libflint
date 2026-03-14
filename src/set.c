#include "lfset.h"

void set_init(Set *set, int (*match)(const void *a, const void *b),
              void (*destroy)(void *data)) {
    ll_init(set, destroy);
    set->match = match;
}

void set_destroy(Set *set) {
    ll_destroy(set);
}

int set_insert(Set *set, const void *data) {
    if (set_is_member(set, data)) {
        return 1;
    }
    return ll_ins_next(set, set->tail, data);
}

int set_remove(Set *set, void **data) {
    ListNode *node = ll_find(set, *data);
    if (node == NULL) {
        return -1;
    }
    return ll_remove(set, node, data);
}

int set_union(Set *setu, const Set *a, const Set *b) {
    ListNode *node;
    void *data;

    set_init(setu, a->match, NULL);
    for (node = a->head; node != NULL; node = node->next) {
        data = node->data;
        if (ll_ins_next(setu, setu->tail, data) != 0) {
            set_destroy(setu);
            return -1;
        }
    }

    for (node = b->head; node != NULL; node = node->next) {
        if (set_is_member(a, node->data)) {
            continue;
        }
        data = node->data;
        if (ll_ins_next(setu, setu->tail, data) != 0) {
            set_destroy(setu);
            return -1;
        }
    }

    return 0;
}

int set_intersection(Set *seti, const Set *a, const Set *b) {
    ListNode *node;
    void *data;

    set_init(seti, a->match, NULL);
    for (node = a->head; node != NULL; node = node->next) {
        data = node->data;
        if (set_is_member(b, data)) {
            if (ll_ins_next(seti, seti->tail, data)) {
                set_destroy(seti);
                return -1;
            }
        }
    }

    return 0;
}

int set_difference(Set *setd, const Set *a, const Set *b) {
    ListNode *node;
    void *data;

    set_init(setd, a->match, NULL);
    for (node = a->head; node != NULL; node = node->next) {
        data = node->data;
        if (!set_is_member(b, data)) {
            if (ll_ins_next(setd, setd->tail, data)) {
                set_destroy(setd);
                return -1;
            }
        }
    }

    return 0;
}

int set_is_member(const Set *set, const void *data) {
    return ll_find((List *)set, data) != NULL;
}

int set_is_subset(const Set *a, const Set *b) {
    if (a->size > b->size) {
        return 0;
    }
    for (ListNode *node = a->head; node != NULL; node = node->next) {
        if (!set_is_member(b, node->data)) {
            return 0;
        }
    }
    return 1;
}

int set_is_equal(const Set *a, const Set *b) {
    if (a->size != b->size) {
        return 0;
    }
    return set_is_subset(a, b);
}
