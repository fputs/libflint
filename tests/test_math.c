#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfmath.h"

int main() {
    /* abs_int */
    ASSERT_EQ(abs_int(5), 5);
    ASSERT_EQ(abs_int(-5), 5);
    ASSERT_EQ(abs_int(0), 0);
    ASSERT_EQ(abs_int(INT_MIN), INT_MAX);

    /* max/min */
    ASSERT_EQ(max_int(1, 2), 2);
    ASSERT_EQ(min_int(1, 2), 1);

    /* binstr_to_int */
    ASSERT_EQ(binstr_to_int("10101101"), 173);
    ASSERT_EQ(binstr_to_int("1010_1101"), 173);
    ASSERT_EQ(binstr_to_int(NULL), -1);
    ASSERT_EQ(binstr_to_int(""), -1);
    ASSERT_EQ(binstr_to_int("_"), -1);
    ASSERT_EQ(binstr_to_int("__"), -1);
    ASSERT_EQ(binstr_to_int("0"), 0);
    ASSERT_EQ(binstr_to_int("2"), -1);
    ASSERT_EQ(binstr_to_int("1111111111111111111111111111111"), INT_MAX);
    ASSERT_EQ(binstr_to_int("10000000000000000000000000000000"), -1);

    /* clamp_int - all branches */
    ASSERT_EQ(clamp_int(10, 2, 5), 5);   /* above high */
    ASSERT_EQ(clamp_int(1, 2, 5), 2);    /* below low */
    ASSERT_EQ(clamp_int(3, 2, 5), 3);    /* in range */
    ASSERT_EQ(clamp_int(3, 5, 2), 3);    /* low > high, swap */
    ASSERT_EQ(clamp_int(10, 5, 2), 5);   /* low > high, above */

    /* gcd */
    ASSERT_EQ(lf_gcd(12, 8), 4);
    ASSERT_EQ(lf_gcd(7, 13), 1);
    ASSERT_EQ(lf_gcd(0, 5), 5);
    ASSERT_EQ(lf_gcd(5, 0), 5);
    ASSERT_EQ(lf_gcd(-12, 8), 4);
    ASSERT_EQ(lf_gcd(-6, -9), 3);

    /* lcm */
    ASSERT_EQ(lf_lcm(4, 6), 12);
    ASSERT_EQ(lf_lcm(7, 13), 91);
    ASSERT_EQ(lf_lcm(1, 5), 5);
    ASSERT_EQ(lf_lcm(0, 5), 0);
    ASSERT_EQ(lf_lcm(INT_MIN, INT_MIN), -1);
    ASSERT_EQ(lf_lcm(INT_MIN, 2), -1);
    ASSERT_EQ(lf_lcm(INT_MAX, INT_MAX), INT_MAX);
    ASSERT_EQ(lf_lcm(INT_MAX, INT_MAX - 1), -1);

    /* lerp */
    ASSERT_TRUE(lf_lerp(0.0f, 10.0f, 0.0f) == 0.0f);
    ASSERT_TRUE(lf_lerp(0.0f, 10.0f, 1.0f) == 10.0f);
    ASSERT_TRUE(lf_lerp(0.0f, 10.0f, 0.5f) == 5.0f);
    ASSERT_TRUE(lf_lerp(2.0f, 4.0f, 0.25f) == 2.5f);

    /* isqrt */
    ASSERT_EQ(lf_isqrt(0), 0);
    ASSERT_EQ(lf_isqrt(1), 1);
    ASSERT_EQ(lf_isqrt(25), 5);
    ASSERT_EQ(lf_isqrt(26), 5);
    ASSERT_EQ(lf_isqrt(100), 10);
    ASSERT_EQ(lf_isqrt(-1), -1);
    ASSERT_EQ(lf_isqrt(INT_MAX), 46340);

    /* LfPoint */
    LfPoint pa = lf_point(3, 7);
    ASSERT_EQ(pa.x, 3);
    ASSERT_EQ(pa.y, 7);
    ASSERT_TRUE(LfPoint_eq(lf_point(1, 2), lf_point(1, 2)));
    ASSERT_FALSE(LfPoint_eq(lf_point(1, 2), lf_point(3, 4)));

    LfPoint ea = lf_point(1, 2);
    LfPoint eb = lf_point(1, 2);
    LfPoint ec = lf_point(9, 9);
    ASSERT_TRUE(LfPoint_eq_p(&ea, &eb));
    ASSERT_FALSE(LfPoint_eq_p(&ea, &ec));
    ASSERT_TRUE(LfPoint_eq_p(NULL, NULL));
    ASSERT_FALSE(LfPoint_eq_p(&ea, NULL));
    ASSERT_EQ(LfPoint_match(&ea, &eb), 0);
    ASSERT_EQ(LfPoint_match(&ea, &ec), 1);

    /* bresenham coordinate verification */
    size_t sz = 0;
    LfPoint *line = bresenham(0, 0, 2, 5, &sz);
    ASSERT_NOT_NULL(line);
    ASSERT_TRUE(sz > 0);
    /* first point */
    ASSERT_EQ(line[0].x, 0);
    ASSERT_EQ(line[0].y, 0);
    /* last point */
    ASSERT_EQ(line[sz - 1].x, 2);
    ASSERT_EQ(line[sz - 1].y, 5);
    free(line);

    /* bresenham single point */
    line = bresenham(3, 3, 3, 3, &sz);
    ASSERT_NOT_NULL(line);
    ASSERT_EQ(sz, 1);
    ASSERT_EQ(line[0].x, 3);
    ASSERT_EQ(line[0].y, 3);
    free(line);

    /* bresenham_p */
    LfPoint p1 = lf_point(0, 0);
    LfPoint p2 = lf_point(4, 0);
    line = bresenham_p(p1, p2, &sz);
    ASSERT_NOT_NULL(line);
    ASSERT_EQ(sz, 5);
    ASSERT_EQ(line[0].x, 0);
    ASSERT_EQ(line[4].x, 4);
    free(line);

    ASSERT_NULL(bresenham(0, 0, 1, 1, NULL));

    TEST_REPORT();
}
