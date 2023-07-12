#ifndef LIBFLINT_SET_H
#define LIBFLINT_SET_H

#include "linkedlist.h"

#define Set List

void set_init(Set *set, int (*match)(const void *a, const void *b),
              void (*destroy)(void *data));

void set_destroy(Set *set);

int set_insert(Set *set, const void *data);

int set_remove(Set *set, void **data);

int set_union(Set *setu, const Set *a, const Set *b);

int set_intersection(Set *seti, const Set *a, const Set *b);

int set_difference(Set *setd, const Set *a, const Set *b);

int set_is_member(const Set *set, const void *data);

int set_is_subset(const Set *a, const Set *b);

int set_is_equal(const Set *a, const Set *b);

#endif
