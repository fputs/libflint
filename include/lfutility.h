#ifndef LIBFLINT_H_UTILITY
#define LIBFLINT_H_UTILITY

typedef struct Point {
    int x;
    int y;
} Point;

Point Point_new(int x, int y);
Point *Point_new_p(int x, int y);
int Point_cmp(Point a, Point b);
int Point_cmp_p(const Point *a, const Point *b);
int Point_cmp_v(const void *a, const void *b);

#endif // LIBFLINT_H_UTILITY
