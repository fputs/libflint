#include <stdlib.h>

#include "lfutility.h"

Point Point_new(int x, int y) {
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

Point *Point_new_p(int x, int y) {
    Point *p = malloc(sizeof(struct Point));
    if (!p) {
        return NULL;
    }
    p->x = x;
    p->y = y;
    return p;
}

void Point_destroy(Point *p) {
    free(p);
}

int Point_eq(const Point a, const Point b) {
    if (a.x == b.x && a.y == b.y) {
        return 1;
    }
    return 0;
}

int Point_cmp_p(const Point *a, const Point *b) {
    if (a->x == b->x && a->y == b->y) {
        return 1;
    }
    return 0;
}

int Point_cmp_v(const void *a, const void *b) {
    return Point_cmp_p(a, b);
}
