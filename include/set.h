#ifndef LIBFLINT_SET_H
#define LIBFLINT_SET_H

#include "linkedlist.h"

#define Set List

void set_init(struct Set* set, int (*match)(const void* a, const void* b),
        void (*destroy)(void* data));
void set_destroy(struct Set* set);
int set_insert(struct Set* set, const void* data);
int set_remove(struct Set* set, void** data);
int set_union(struct Set* setu, const struct Set* a, const struct Set* b);
int set_intersection(struct Set* seti, const struct Set* a, const struct Set* b);
int set_difference(struct Set* setd, const struct Set* a, const struct Set* b);
int set_is_member(const struct Set* set, const void* data);
int set_is_subset(const struct Set* a, const struct Set* b);
int set_is_equal(const struct Set* a, const struct Set* b);

#endif
