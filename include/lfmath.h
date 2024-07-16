#ifndef LIBFLINT_H_MATH
#define LIBFLINT_H_MATH

#include <stddef.h>

#include "lfutility.h"

int max_int(int a, int b);

int min_int(int a, int b);

int clamp_int(int i, int low, int high);

int binstr_to_int(const char *s);

int is_power_of_two(int i);

Point *bresenham(int x0, int y0, int x1, int y1, size_t *sz);

Point *bresenham_p(Point p1, Point p2, size_t *sz);

#endif // LIBFLINT_H_MATH
