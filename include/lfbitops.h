#ifndef LIBFLINT_BITOPS_H
#define LIBFLINT_BITOPS_H

#define LF_BIT_SET(x, i) ((x) |= 1UL << (i))
#define LF_BIT_CLEAR(x, i) ((x) &= ~(1UL << (i)))
#define LF_BIT_TOGGLE(x, i) ((x) ^= (1UL << (i)))
#define LF_BIT_TEST(x, i) ((x) & 1UL << (i))

#define LF_BIT_EXTRACT(x, start, width) \
    (((x) >>(start)) & ((1UL <<(width)) - 1UL))

#define LF_BIT_INSERT(x, start, width, val) \
    ((x) = ((x) & ~(((1UL << (width)) - 1) << (start))) | \
        (((val) & ((1UL << (width)) - 1)) << (start)))

#endif //LIBFLINT_BITOPS_H
