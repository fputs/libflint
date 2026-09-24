#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lfcompat.h"
#include "lfprocess.h"

char *lf_capture_system(const char *cmd, size_t buf_sz) {
    if (cmd == NULL) {
        return NULL;
    }

    if (buf_sz == 0) {
        buf_sz = LF_CAPTURE_SYSTEM_BUFSIZE;
    }
    if (buf_sz < 2) {
        return NULL;
    }

    FILE *tmp = popen(cmd, "r");
    if (tmp == NULL) {
        return NULL;
    }

    size_t total = 0;
    size_t cap = buf_sz;
    char *buf = malloc(cap);
    if (buf == NULL) {
        pclose(tmp);
        return NULL;
    }

    size_t nread;
    while ((nread = fread(buf + total, 1, cap - total - 1, tmp)) > 0) {
        total += nread;
        if (total + 1 >= cap) {
            if (cap > SIZE_MAX / 2) {
                free(buf);
                pclose(tmp);
                return NULL;
            }
            size_t ncap = cap * 2;
            char *tmp2 = lf_reallocarray(buf, ncap, 1);
            if (tmp2 == NULL) {
                free(buf);
                pclose(tmp);
                return NULL;
            }
            buf = tmp2;
            cap = ncap;
        }
    }

    int read_error = ferror(tmp);
    int close_status = pclose(tmp);
    if (read_error || close_status == -1) {
        free(buf);
        return NULL;
    }
    buf[total] = '\0';
    return buf;
}
