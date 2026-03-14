#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfmath.h"

int main() {
    int i = 1, j = 2;
    ASSERT_EQ(max_int(i, j), j);
    ASSERT_EQ(min_int(i, j), i);

    char *s = "10101101";
    ASSERT_EQ(binstr_to_int(s), 173);

    char *s2 = "1010_1101";
    ASSERT_EQ(binstr_to_int(s2), 173);

    i = 10;
    i = clamp_int(i, 2, 5);
    ASSERT_EQ(i, 5);

    /* abs_int */
    ASSERT_EQ(abs_int(5), 5);
    ASSERT_EQ(abs_int(-5), 5);
    ASSERT_EQ(abs_int(0), 0);

    /* gcd */
    ASSERT_EQ(gcd(12, 8), 4);
    ASSERT_EQ(gcd(7, 13), 1);
    ASSERT_EQ(gcd(0, 5), 5);
    ASSERT_EQ(gcd(5, 0), 5);
    ASSERT_EQ(gcd(-12, 8), 4);
    ASSERT_EQ(gcd(-6, -9), 3);

    /* lcm */
    ASSERT_EQ(lcm(4, 6), 12);
    ASSERT_EQ(lcm(7, 13), 91);
    ASSERT_EQ(lcm(1, 5), 5);
    ASSERT_EQ(lcm(0, 5), 0);

    /* lerp */
    ASSERT_TRUE(lerp(0.0f, 10.0f, 0.0f) == 0.0f);
    ASSERT_TRUE(lerp(0.0f, 10.0f, 1.0f) == 10.0f);
    ASSERT_TRUE(lerp(0.0f, 10.0f, 0.5f) == 5.0f);
    ASSERT_TRUE(lerp(2.0f, 4.0f, 0.25f) == 2.5f);

    /* isqrt */
    ASSERT_EQ(isqrt(0), 0);
    ASSERT_EQ(isqrt(1), 1);
    ASSERT_EQ(isqrt(25), 5);
    ASSERT_EQ(isqrt(26), 5);
    ASSERT_EQ(isqrt(100), 10);
    ASSERT_EQ(isqrt(-1), -1);

    size_t sz = 0;
    Point *line = bresenham(0, 0, 2, 5, &sz);
    ASSERT_TRUE(sz > 0);
    ASSERT_NOT_NULL(line);
    free(line);

    TEST_REPORT();
}
