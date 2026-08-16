#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "lfcompat.h"
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
    if (s == NULL) {
        return -1;
    }
    int n = 0;
    int saw_digit = 0;
    for (size_t i = 0; s[i] != '\0'; ++i) {
        if (s[i] == '_') {
            continue;
        }
        if (s[i] != '0' && s[i] != '1') {
            return -1;
        }
        saw_digit = 1;
        if (n > INT_MAX / 2) {
            return -1;
        }
        n *= 2;
        if (s[i] == '1') {
            if (n == INT_MAX) {
                return -1;
            }
            n += 1;
        }
    }
    if (!saw_digit) {
        return -1;
    }
    return n;
}

Point *bresenham(int x0, int y0, int x1, int y1, size_t *sz) {
    if (sz == NULL) {
        return NULL;
    }
    *sz = 0;

    long long dx = (long long)x1 - (long long)x0;
    if (dx < 0) {
        dx = -dx;
    }
    long long dy = (long long)y1 - (long long)y0;
    if (dy < 0) {
        dy = -dy;
    }

    long long span = dx > dy ? dx : dy;
    if (span < 0 || (unsigned long long)span > (unsigned long long)SIZE_MAX - 1) {
        return NULL;
    }
    size_t npts = (size_t)span + 1;

    Point *line = lf_reallocarray(NULL, npts, sizeof(Point));
    if (line == NULL) {
        return NULL;
    }

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    long long err = (dx > dy ? dx : -dy) / 2;
    size_t n = 0;
    int x = x0;
    int y = y0;

    for (;;) {
        line[n].x = x;
        line[n].y = y;
        n++;

        if (x == x1 && y == y1) {
            break;
        }

        long long e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x += sx;
        }
        if (e2 < dy) {
            err += dx;
            y += sy;
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
    if (a == INT_MIN || b == INT_MIN) {
        return -1;
    }
    int ga = abs_int(a);
    int gb = abs_int(b);
    int d = gcd(ga, gb);
    int q = ga / d;
    if (q != 0 && gb > INT_MAX / q) {
        return -1;
    }
    return q * gb;
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
    unsigned int un = (unsigned int)n;
    unsigned int x = un;
    unsigned int y = (x + 1u) / 2u;
    while (y < x) {
        x = y;
        y = (x + un / x) / 2u;
    }
    return (int)x;
}
