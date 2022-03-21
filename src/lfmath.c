#include <string.h>

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
    for (int i = (int)strlen(s) - 1; i >= 0; --i) {
        if (s[i] == '1') {
            n += m;
        }
        m *= 2;
    }
    return n;
}
