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

    size_t sz = 0;
    Point *line = bresenham(0, 0, 2, 5, &sz);
    ASSERT_TRUE(sz > 0);
    ASSERT_NOT_NULL(line);
    free(line);

    TEST_REPORT();
}
