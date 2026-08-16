#include <stdint.h>
#include <stdio.h>
#include "lftest.h"
#include "lfbitops.h"

int main() {
    uint32_t x;

    /* LF_BIT_SET */
    x = 0;
    LF_BIT_SET(x, 0);
    ASSERT_EQ(x, 1);

    x = 0;
    LF_BIT_SET(x, 7);
    ASSERT_EQ(x, 0x80);

    x = 0x0F;
    LF_BIT_SET(x, 3);
    ASSERT_EQ(x, 0x0F); /* already set, no change */

    x = 0;
    LF_BIT_SET(x, 31);
    ASSERT_EQ(x, (uint32_t)1 << 31);

    /* LF_BIT_CLEAR */
    x = 0xFF;
    LF_BIT_CLEAR(x, 0);
    ASSERT_EQ(x, 0xFE);

    x = 0xFF;
    LF_BIT_CLEAR(x, 7);
    ASSERT_EQ(x, 0x7F);

    x = 0xF0;
    LF_BIT_CLEAR(x, 0);
    ASSERT_EQ(x, 0xF0); /* already clear, no change */

    /* LF_BIT_TOGGLE */
    x = 0;
    LF_BIT_TOGGLE(x, 4);
    ASSERT_EQ(x, 0x10);

    LF_BIT_TOGGLE(x, 4);
    ASSERT_EQ(x, 0); /* toggle back off */

    x = 0xAA; /* 10101010 */
    LF_BIT_TOGGLE(x, 0);
    ASSERT_EQ(x, 0xAB); /* 10101011 */

    /* LF_BIT_TEST */
    x = 0xA5; /* 10100101 */
    ASSERT_TRUE(LF_BIT_TEST(x, 0));
    ASSERT_FALSE(LF_BIT_TEST(x, 1));
    ASSERT_TRUE(LF_BIT_TEST(x, 2));
    ASSERT_FALSE(LF_BIT_TEST(x, 3));
    ASSERT_FALSE(LF_BIT_TEST(x, 4));
    ASSERT_TRUE(LF_BIT_TEST(x, 5));
    ASSERT_FALSE(LF_BIT_TEST(x, 6));
    ASSERT_TRUE(LF_BIT_TEST(x, 7));
    ASSERT_EQ(x, 0xA5); /* test must not modify x */

    /* LF_BIT_EXTRACT */
    x = 0xDEADBEEF;
    ASSERT_EQ(LF_BIT_EXTRACT(x, 0, 4), 0xF);    /* low nibble */
    ASSERT_EQ(LF_BIT_EXTRACT(x, 4, 4), 0xE);    /* second nibble */
    ASSERT_EQ(LF_BIT_EXTRACT(x, 0, 8), 0xEF);   /* low byte */
    ASSERT_EQ(LF_BIT_EXTRACT(x, 8, 8), 0xBE);   /* second byte */
    ASSERT_EQ(LF_BIT_EXTRACT(x, 16, 16), 0xDEAD); /* upper half */

    /* extract single bit */
    x = 0x80;
    ASSERT_EQ(LF_BIT_EXTRACT(x, 7, 1), 1);
    ASSERT_EQ(LF_BIT_EXTRACT(x, 6, 1), 0);

    /* full-width extract is defined (1UL << 32 is UB on ILP32) */
    ASSERT_EQ(LF_BIT_EXTRACT(x, 0, 32), (uint32_t)0x80);
    {
        uint64_t y = ~0ULL;
        ASSERT_EQ(LF_BIT_EXTRACT(y, 0, 32), 0xFFFFFFFFULL);
        ASSERT_EQ(LF_BIT_EXTRACT(y, 0, 64), ~0ULL);
        ASSERT_EQ(LF_BIT_EXTRACT(y, 32, 32), 0xFFFFFFFFULL);
    }

    /* LF_BIT_INSERT */
    x = 0;
    LF_BIT_INSERT(x, 0, 4, 0xA);
    ASSERT_EQ(x, 0x0A);

    LF_BIT_INSERT(x, 4, 4, 0xB);
    ASSERT_EQ(x, 0xBA);

    /* overwrite existing field */
    x = 0xFF;
    LF_BIT_INSERT(x, 0, 4, 0x3);
    ASSERT_EQ(x, 0xF3);

    /* insert should not affect bits outside the field */
    x = 0xFFFF0000;
    LF_BIT_INSERT(x, 8, 8, 0xAB);
    ASSERT_EQ(x, 0xFFFFAB00);

    /* oversized value gets clipped to field width */
    x = 0;
    LF_BIT_INSERT(x, 0, 3, 0xFF);
    ASSERT_EQ(x, 0x07); /* only low 3 bits of 0xFF */

    /* round-trip: insert then extract */
    x = 0;
    LF_BIT_INSERT(x, 12, 8, 0x42);
    ASSERT_EQ(LF_BIT_EXTRACT(x, 12, 8), 0x42);

    {
        uint64_t z = 0;
        LF_BIT_SET(z, 63);
        ASSERT_EQ(z, 1ULL << 63);
        LF_BIT_INSERT(z, 0, 32, 0xFFFFFFFFULL);
        ASSERT_EQ(z, (1ULL << 63) | 0xFFFFFFFFULL);
        LF_BIT_INSERT(z, 32, 32, 0xA5A5A5A5ULL);
        ASSERT_EQ(z, 0xA5A5A5A5FFFFFFFFULL);
    }

    /* lf_is_pow2 */
    ASSERT_TRUE(lf_is_pow2(1));
    ASSERT_TRUE(lf_is_pow2(2));
    ASSERT_TRUE(lf_is_pow2(4));
    ASSERT_TRUE(lf_is_pow2(1024));
    ASSERT_TRUE(lf_is_pow2(1ULL << 63));
    ASSERT_FALSE(lf_is_pow2(0));
    ASSERT_FALSE(lf_is_pow2(3));
    ASSERT_FALSE(lf_is_pow2((uint64_t)-1));

    TEST_REPORT();
}
