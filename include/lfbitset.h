#ifndef LIBFLINT_H_BITSET
#define LIBFLINT_H_BITSET
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t *words;
    size_t nbits, nwords;
} LfBitSet;

int lf_bitset_init(LfBitSet *set, size_t nbits);
/* 0 success, -1 invalid handle/index; test output unchanged on failure. */
int lf_bitset_set(LfBitSet *set, size_t index);
int lf_bitset_clear(LfBitSet *set, size_t index);
int lf_bitset_toggle(LfBitSet *set, size_t index);
int lf_bitset_test(const LfBitSet *set, size_t index, int *value);
size_t lf_bitset_count(const LfBitSet *set);
/* In-place operations require equal sizes; -1 leaves destination unchanged. */
int lf_bitset_union(LfBitSet *dest, const LfBitSet *src);
int lf_bitset_intersection(LfBitSet *dest, const LfBitSet *src);
void lf_bitset_destroy(LfBitSet *set);
#endif
