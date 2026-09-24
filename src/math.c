#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "lfcompat.h"
#include "lfmath.h"

LfPoint lf_point(int x, int y) {
    LfPoint p;
    p.x = x;
    p.y = y;
    return p;
}

int LfPoint_eq(LfPoint a, LfPoint b) {
    return a.x == b.x && a.y == b.y;
}

int LfPoint_eq_p(const LfPoint *a, const LfPoint *b) {
    if (a == NULL || b == NULL) {
        return a == b;
    }
    return LfPoint_eq(*a, *b);
}

int LfPoint_match(const void *a, const void *b) {
    return LfPoint_eq_p(a, b) ? 0 : 1;
}

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

LfPoint *bresenham(int x0, int y0, int x1, int y1, size_t *sz) {
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

    LfPoint *line = lf_reallocarray(NULL, npts, sizeof(LfPoint));
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

LfPoint *bresenham_p(LfPoint p1, LfPoint p2, size_t *sz) {
    return bresenham(p1.x, p1.y, p2.x, p2.y, sz);
}

int lf_gcd(int a, int b) {
    unsigned int ua = a < 0 ? 0u - (unsigned int)a : (unsigned int)a;
    unsigned int ub = b < 0 ? 0u - (unsigned int)b : (unsigned int)b;
    while (ub) {
        unsigned int tmp = ub;
        ub = ua % ub;
        ua = tmp;
    }
    return ua > INT_MAX ? -1 : (int)ua;
}

int lf_lcm(int a, int b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    if (a == INT_MIN || b == INT_MIN) {
        return -1;
    }
    int ga = abs_int(a);
    int gb = abs_int(b);
    int d = lf_gcd(ga, gb);
    int q = ga / d;
    if (q != 0 && gb > INT_MAX / q) {
        return -1;
    }
    return q * gb;
}

float lf_lerp(float a, float b, float t) {
    if (t == 0.0f) {
        return a;
    }
    if (t == 1.0f) {
        return b;
    }
    if ((a <= 0.0f && b >= 0.0f) || (a >= 0.0f && b <= 0.0f)) {
        return (1.0f - t) * a + t * b;
    }
    return a + t * (b - a);
}

int lf_isqrt(int n) {
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
