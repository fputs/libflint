#include "lfbitset.h"
#include "lfcompat.h"
#include <stdlib.h>
#include <string.h>

static void mask_tail(LfBitSet *s) {
    size_t bits = s->nbits % 64;
    if (bits) s->words[s->nwords - 1] &= (UINT64_C(1) << bits) - 1;
}

int lf_bitset_init(LfBitSet *s, size_t nbits) {
    if (!s) return -1;
    memset(s, 0, sizeof(*s));
    size_t n = nbits / 64 + (nbits % 64 != 0);
    if (n) {
        s->words = lf_reallocarray(NULL, n, sizeof(*s->words));
        if (!s->words) return -1;
        memset(s->words, 0, n * sizeof(*s->words));
    }
    s->nwords = n;
    s->nbits = nbits;
    return 0;
}

int lf_bitset_set(LfBitSet *s, size_t index) {
    if (!s || index >= s->nbits) return -1;
    s->words[index / 64] |= UINT64_C(1) << (index % 64);
    mask_tail(s);
    return 0;
}

int lf_bitset_clear(LfBitSet *s, size_t index) {
    if (!s || index >= s->nbits) return -1;
    s->words[index / 64] &= ~(UINT64_C(1) << (index % 64));
    mask_tail(s);
    return 0;
}

int lf_bitset_toggle(LfBitSet *s, size_t index) {
    if (!s || index >= s->nbits) return -1;
    s->words[index / 64] ^= UINT64_C(1) << (index % 64);
    mask_tail(s);
    return 0;
}

int lf_bitset_test(const LfBitSet *s, size_t index, int *value) {
    if (!s || !value || index >= s->nbits) return -1;
    *value = (int)((s->words[index / 64] >> (index % 64)) & 1);
    return 0;
}

size_t lf_bitset_count(const LfBitSet *s) {
    if (!s) return 0;
    size_t count = 0;
    for (size_t i = 0; i < s->nwords; i++) {
        uint64_t word = s->words[i];
        if (i == s->nwords - 1 && s->nbits % 64)
            word &= (UINT64_C(1) << (s->nbits % 64)) - 1;
        while (word) { word &= word - 1; count++; }
    }
    return count;
}

int lf_bitset_union(LfBitSet *dest, const LfBitSet *src) {
    if (!dest || !src || dest->nbits != src->nbits) return -1;
    for (size_t i = 0; i < dest->nwords; i++) dest->words[i] |= src->words[i];
    mask_tail(dest);
    return 0;
}

int lf_bitset_intersection(LfBitSet *dest, const LfBitSet *src) {
    if (!dest || !src || dest->nbits != src->nbits) return -1;
    for (size_t i = 0; i < dest->nwords; i++) dest->words[i] &= src->words[i];
    mask_tail(dest);
    return 0;
}

void lf_bitset_destroy(LfBitSet *s) {
    if (!s) return;
    free(s->words);
    memset(s, 0, sizeof(*s));
}
