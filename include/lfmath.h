#ifndef LIBFLINT_H_MATH
#define LIBFLINT_H_MATH

#include <stddef.h>

typedef struct {
    int x;
    int y;
} LfPoint;

LfPoint lf_point(int x, int y);

/* 1 if equal, 0 otherwise. Not a qsort comparator. */
int LfPoint_eq(LfPoint a, LfPoint b);

int LfPoint_eq_p(const LfPoint *a, const LfPoint *b);

/* 0 if equal, 1 otherwise. Safe as a List/Set match callback. */
int LfPoint_match(const void *a, const void *b);

int abs_int(int a);

int max_int(int a, int b);

int min_int(int a, int b);

int clamp_int(int i, int low, int high);

int binstr_to_int(const char *s);

int lf_gcd(int a, int b);

int lf_lcm(int a, int b);

float lf_lerp(float a, float b, float t);

int lf_isqrt(int n);

LfPoint *bresenham(int x0, int y0, int x1, int y1, size_t *sz);

LfPoint *bresenham_p(LfPoint p1, LfPoint p2, size_t *sz);

#endif /* LIBFLINT_H_MATH */
