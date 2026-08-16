#ifndef LIBFLINT_H_HASHSET
#define LIBFLINT_H_HASHSET

#include <stddef.h>

/*
 * Open-addressed uniqueness-constrained collection of void *.
 * match returns 0 if equal (same convention as Set). hash is the
 * caller-provided mixer. Membership is amortized O(1).
 */
typedef struct {
    void **slots;
    unsigned char *state;
    size_t cap;
    size_t len;
    size_t tombs;
    int (*match)(const void *a, const void *b);
    size_t (*hash)(const void *data);
    void (*destroy)(void *data);
} LfHashSet;

#define lf_hashset_size(s) ((s)->len)

int lf_hashset_init(LfHashSet *set, size_t cap,
                    int (*match)(const void *a, const void *b),
                    size_t (*hash)(const void *data),
                    void (*destroy)(void *data));

void lf_hashset_destroy(LfHashSet *set);

/* Returns 0 on insert, 1 if data is a duplicate, -1 on error */
int lf_hashset_insert(LfHashSet *set, const void *data);

int lf_hashset_remove(LfHashSet *set, void **data);

int lf_hashset_is_member(const LfHashSet *set, const void *data);

#endif /* LIBFLINT_H_HASHSET */
