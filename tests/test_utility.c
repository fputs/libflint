#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfutility.h"

int main() {
    /* Point_new */
    Point p = Point_new(3, 7);
    ASSERT_EQ(p.x, 3);
    ASSERT_EQ(p.y, 7);

    /* Point_new_p */
    Point *pp = Point_new_p(5, 11);
    ASSERT_NOT_NULL(pp);
    ASSERT_EQ(pp->x, 5);
    ASSERT_EQ(pp->y, 11);

    /* Point_eq */
    Point a = Point_new(1, 2);
    Point b = Point_new(1, 2);
    Point c = Point_new(3, 4);
    ASSERT_TRUE(Point_eq(a, b));
    ASSERT_FALSE(Point_eq(a, c));

    /* Point_cmp_p */
    Point *pa = Point_new_p(1, 2);
    Point *pb = Point_new_p(1, 2);
    Point *pc = Point_new_p(9, 9);
    ASSERT_NOT_NULL(pa);
    ASSERT_NOT_NULL(pb);
    ASSERT_NOT_NULL(pc);
    ASSERT_TRUE(Point_cmp_p(pa, pb));
    ASSERT_FALSE(Point_cmp_p(pa, pc));

    /* Point_cmp_v */
    ASSERT_TRUE(Point_cmp_v(pa, pb));
    ASSERT_FALSE(Point_cmp_v(pa, pc));

    /* Point_destroy */
    Point_destroy(pp);
    Point_destroy(pa);
    Point_destroy(pb);
    Point_destroy(pc);
    Point_destroy(NULL); /* should not crash */

    TEST_REPORT();
}
