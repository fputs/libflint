#ifndef LIBFLINT_SET_H
#define LIBFLINT_SET_H

#include "lflinkedlist.h"

/*
 * Small uniqueness-constrained collection over a List.
 * Membership is O(n). The wrapper type is what keeps ll_ins_next
 * from compiling against a Set and breaking uniqueness.
 */
typedef struct {
    List impl;
} Set;

#define set_size(s) ((s)->impl.size)

int set_init(Set *set, int (*match)(const void *a, const void *b),
             void (*destroy)(void *data));

void set_destroy(Set *set);

/* Returns 0 on success, 1 if data is a duplicate, -1 on error */
int set_insert(Set *set, const void *data);

int set_remove(Set *set, void **data);

/*
 * Borrowed algebra: dest must be caller-initialized, empty, same match
 * as a and b, and destroy == NULL. Payloads alias a/b. Reject dest == a
 * or dest == b. Do not destroy a or b while dest is live.
 */
int set_union(Set *dest, const Set *a, const Set *b);

int set_intersection(Set *dest, const Set *a, const Set *b);

int set_difference(Set *dest, const Set *a, const Set *b);

int set_is_member(const Set *set, const void *data);

int set_is_subset(const Set *a, const Set *b);

int set_is_equal(const Set *a, const Set *b);

#endif
