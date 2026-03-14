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

    /* clamp_int - all branches */
    ASSERT_EQ(clamp_int(10, 2, 5), 5);   /* above high */
    ASSERT_EQ(clamp_int(1, 2, 5), 2);    /* below low */
    ASSERT_EQ(clamp_int(3, 2, 5), 3);    /* in range */
    ASSERT_EQ(clamp_int(3, 5, 2), 3);    /* low > high, swap */
    ASSERT_EQ(clamp_int(10, 5, 2), 5);   /* low > high, above */

    /* is_power_of_two */
    ASSERT_TRUE(is_power_of_two(1));
    ASSERT_TRUE(is_power_of_two(2));
    ASSERT_TRUE(is_power_of_two(4));
    ASSERT_TRUE(is_power_of_two(1024));
    ASSERT_FALSE(is_power_of_two(0));
    ASSERT_FALSE(is_power_of_two(3));
    ASSERT_FALSE(is_power_of_two(-1));

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

    /* bresenham coordinate verification */
    size_t sz = 0;
    Point *line = bresenham(0, 0, 2, 5, &sz);
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
    Point p1 = Point_new(0, 0);
    Point p2 = Point_new(4, 0);
    line = bresenham_p(p1, p2, &sz);
    ASSERT_NOT_NULL(line);
    ASSERT_EQ(sz, 5);
    ASSERT_EQ(line[0].x, 0);
    ASSERT_EQ(line[4].x, 4);
    free(line);

    TEST_REPORT();
}
