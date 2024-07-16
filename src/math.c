#include <string.h>
#include <stdlib.h>

#include "lfmath.h"

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
    if (i > high) {
        return high;
    } else if (i < low) {
        return low;
    }
    return i;
}

int binstr_to_int(const char *s) {
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
    Point *line = NULL;
    size_t n = 0;
    *sz = 0;

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;) {
        line = realloc(line, sizeof(Point) * ++n);
        ++*sz;
        line[n - 1].x = x0;
        line[n - 1].y = y0;

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

    return line;
}

Point *bresenham_p(Point p1, Point p2, size_t *sz) {
    return bresenham(p1.x, p1.y, p2.x, p2.y, sz);
}

int is_power_of_two(int i) {
    return (i & (i - 1)) == 0;
}
