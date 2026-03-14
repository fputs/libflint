#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "lfmath.h"

int abs_int(int a) {
    if (a == INT_MIN) {
        return INT_MAX;
    }
    return a < 0 ? -a : a;
}

int max_int(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int min_int(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int clamp_int(int i, int low, int high) {
    if (low > high) {
        int tmp = low;
        low = high;
        high = tmp;
    }
    if (i > high) {
        return high;
    } else if (i < low) {
        return low;
    }
    return i;
}

int binstr_to_int(const char *s) {
    if (!s) {
        return -1;
    }
    int n = 0, m = 1;
    for (int i = (int) strlen(s) - 1; i >= 0; --i) {
        if (s[i] == '_') {
            continue;
        }
        if (s[i] == '1') {
            n += m;
        }
        m *= 2;
    }
    return n;
}

Point *bresenham(int x0, int y0, int x1, int y1, size_t *sz) {
    *sz = 0;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    size_t npts = (size_t)(dx > dy ? dx : dy) + 1;

    Point *line = malloc(sizeof(Point) * npts);
    if (!line) {
        return NULL;
    }

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    size_t n = 0;

    for (;;) {
        line[n].x = x0;
        line[n].y = y0;
        n++;

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }

    *sz = n;
    return line;
}

Point *bresenham_p(Point p1, Point p2, size_t *sz) {
    return bresenham(p1.x, p1.y, p2.x, p2.y, sz);
}

int is_power_of_two(int i) {
    return i > 0 && (i & (i - 1)) == 0;
}

int gcd(int a, int b) {
    a = abs_int(a);
    b = abs_int(b);
    while (b) {
        int tmp = b;
        b = a % b;
        a = tmp;
    }
    return a;
}

int lcm(int a, int b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    return abs_int(a / gcd(a, b) * b);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

int isqrt(int n) {
    if (n < 0) {
        return -1;
    }
    if (n < 2) {
        return n;
    }
    int x = n;
    int y = (x + 1) / 2;
    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }
    return x;
}
