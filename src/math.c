#include <string.h>

#include "math.h"

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

int binstr_to_int(const char *s) {
    int n = 0, m = 1;
    for (size_t i = strlen(s) - 1; i >= 0; --i) {
        if (s[i] == '1') {
            n += m;
        }
        m *= 2;
    }
    return n;
}
