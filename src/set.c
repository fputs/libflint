#include "set.h"

void set_init(struct Set* set, int (*match)(const void* a, const void* b),
              void (*destroy)(void* data)) {
    ll_init(set, destroy);
    set->match = match;
}

void set_destroy(struct Set* set) {
    ll_destroy(set);
}

int set_insert(struct Set* set, const void* data) {
    if (set_is_member(set, data)) {
        return 1;
    }
    return ll_ins_next(set, set->tail, data);
}

int set_remove(struct Set* set, void** data) {
    struct ListNode* node = NULL;

    for (node = set->head; node != NULL; node = node->next) {
        if (set->match(*data, node->data)) {
            break;
        }
    }

    if (node == NULL) {
        return -1;
    }
    return ll_remove_next(set, node, data);
}

int set_union(struct Set* setu, const struct Set* a, const struct Set* b) {
    struct ListNode* node;
    void* data;

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

int set_intersection(struct Set* seti, const struct Set* a, const struct Set* b) {
    struct ListNode* node;
    void* data;

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

int set_difference(struct Set* setd, const struct Set* a, const struct Set* b) {
    struct ListNode* node;
    void* data;

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

int set_is_member(const struct Set* set, const void* data) {
    for (struct ListNode* node = set->head; node != NULL; node = node->next) {
        if (set->match(data, node->data)) {
            return 1;
        }
    }
    return 0;
}

int set_is_subset(const struct Set* a, const struct Set* b) {
    if (a->size > b->size) {
        return 0;
    }
    for (struct ListNode* node = a->head; node != NULL; node = node->next) {
        if (!set_is_member(b, node->data)) {
            return 0;
        }
    }
    return 1;
}

int set_is_equal(const struct Set* a, const struct Set* b) {
    if (a->size == b->size) {
        return 0;
    }
    return set_is_subset(a, b);
}
