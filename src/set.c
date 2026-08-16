#include "lfset.h"

int set_init(Set *set, int (*match)(const void *a, const void *b),
             void (*destroy)(void *data)) {
    if (set == NULL || match == NULL) {
        return -1;
    }
    ll_init(&set->impl, destroy, match);
    return 0;
}

void set_destroy(Set *set) {
    if (set == NULL) {
        return;
    }
    ll_destroy(&set->impl);
}

int set_insert(Set *set, const void *data) {
    if (set == NULL) {
        return -1;
    }
    if (set_is_member(set, data)) {
        return 1;
    }
    return ll_ins_next(&set->impl, set->impl.tail, data);
}

int set_remove(Set *set, void **data) {
    if (set == NULL || data == NULL) {
        return -1;
    }
    ListNode *node = ll_find(&set->impl, *data);
    if (node == NULL) {
        return -1;
    }
    return ll_remove(&set->impl, node, data);
}

static int algebra_ok(Set *dest, const Set *a, const Set *b) {
    if (dest == NULL || a == NULL || b == NULL) {
        return 0;
    }
    if (dest == a || dest == b) {
        return 0;
    }
    if (a->impl.match == NULL || a->impl.match != b->impl.match) {
        return 0;
    }
    if (dest->impl.match != a->impl.match) {
        return 0;
    }
    if (dest->impl.destroy != NULL) {
        return 0;
    }
    if (dest->impl.size != 0 || dest->impl.head != NULL) {
        return 0;
    }
    return 1;
}

/* Drop borrowed nodes without touching payloads; dest stays initialized. */
static void algebra_unwind(Set *dest) {
    while (dest->impl.size > 0) {
        void *dummy;
        if (ll_remove(&dest->impl, dest->impl.tail, &dummy) != 0) {
            break;
        }
    }
}

static int insert_alias(Set *dest, void *data) {
    if (set_is_member(dest, data)) {
        return 0;
    }
    return ll_ins_next(&dest->impl, dest->impl.tail, data);
}

int set_union(Set *dest, const Set *a, const Set *b) {
    ListNode *node;

    if (!algebra_ok(dest, a, b)) {
        return -1;
    }

    for (node = a->impl.head; node != NULL; node = node->next) {
        if (insert_alias(dest, node->data) != 0) {
            algebra_unwind(dest);
            return -1;
        }
    }

    for (node = b->impl.head; node != NULL; node = node->next) {
        if (insert_alias(dest, node->data) != 0) {
            algebra_unwind(dest);
            return -1;
        }
    }

    return 0;
}

int set_intersection(Set *dest, const Set *a, const Set *b) {
    ListNode *node;

    if (!algebra_ok(dest, a, b)) {
        return -1;
    }

    for (node = a->impl.head; node != NULL; node = node->next) {
        if (set_is_member(b, node->data)) {
            if (insert_alias(dest, node->data) != 0) {
                algebra_unwind(dest);
                return -1;
            }
        }
    }

    return 0;
}

int set_difference(Set *dest, const Set *a, const Set *b) {
    ListNode *node;

    if (!algebra_ok(dest, a, b)) {
        return -1;
    }

    for (node = a->impl.head; node != NULL; node = node->next) {
        if (!set_is_member(b, node->data)) {
            if (insert_alias(dest, node->data) != 0) {
                algebra_unwind(dest);
                return -1;
            }
        }
    }

    return 0;
}

int set_is_member(const Set *set, const void *data) {
    if (set == NULL) {
        return 0;
    }
    return ll_find(&set->impl, data) != NULL;
}

int set_is_subset(const Set *a, const Set *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }
    if (a->impl.size > b->impl.size) {
        return 0;
    }
    for (ListNode *node = a->impl.head; node != NULL; node = node->next) {
        if (!set_is_member(b, node->data)) {
            return 0;
        }
    }
    return 1;
}

int set_is_equal(const Set *a, const Set *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }
    if (a->impl.size != b->impl.size) {
        return 0;
    }
    return set_is_subset(a, b);
}
