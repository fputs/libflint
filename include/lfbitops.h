#ifndef LIBFLINT_BITOPS_H
#define LIBFLINT_BITOPS_H

#include <stdint.h>

/*
 * Arguments are evaluated more than once. Do not write
 * LF_BIT_SET(arr[i++], 3) or LF_BIT_EXTRACT(x, start++, width).
 * Bit index i must be 0..63. width 64 is a full-width mask.
 */

static inline uint64_t lf_bit_mask(unsigned width)
{
    return width >= 64u ? ~0ULL : ((1ULL << width) - 1ULL);
}

#define LF_BIT_SET(x, i) ((x) |= 1ULL << (i))
#define LF_BIT_CLEAR(x, i) ((x) &= ~(1ULL << (i)))
#define LF_BIT_TOGGLE(x, i) ((x) ^= (1ULL << (i)))
#define LF_BIT_TEST(x, i) ((x) & (1ULL << (i)))

#define LF_BIT_EXTRACT(x, start, width) \
    (((x) >> (start)) & lf_bit_mask((unsigned)(width)))

#define LF_BIT_INSERT(x, start, width, val) \
    ((x) = ((x) & ~(lf_bit_mask((unsigned)(width)) << (start))) | \
        (((val) & lf_bit_mask((unsigned)(width))) << (start)))

#endif //LIBFLINT_BITOPS_H
