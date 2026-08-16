#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#ifdef __linux__

#include <bsd/stdlib.h>

#endif

#include "lfcompat.h"
#include "lfinput.h"

static FILE *inp_open_file(const char *path, size_t *fsz, const char *mode) {
    if (path == NULL || fsz == NULL || mode == NULL) {
        return NULL;
    }

    FILE *fp = fopen(path, mode);
    if (fp == NULL) {
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }
    long pos = ftell(fp);
    if (pos < 0) {
        fclose(fp);
        return NULL;
    }
    *fsz = (size_t)pos;
    rewind(fp);

    return fp;
}

unsigned char *inp_get_binary(const char *path, size_t *fsz) {
    FILE *fp = inp_open_file(path, fsz, "rb");
    if (fp == NULL) {
        return NULL;
    }

    unsigned char *buf = malloc(*fsz == 0 ? 1 : *fsz);
    if (buf == NULL) {
        fclose(fp);
        return NULL;
    }

    if (*fsz > 0 && fread(buf, 1, *fsz, fp) != *fsz) {
        free(buf);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    return buf;
}

char *inp_get_input(const char *path) {
    size_t fsz = 0;
    FILE *fp = inp_open_file(path, &fsz, "r");
    if (fp == NULL) {
        return NULL;
    }

    char *buf = malloc(fsz + 1);
    if (buf == NULL) {
        fclose(fp);
        return NULL;
    }

    if (fsz > 0 && fread(buf, 1, fsz, fp) != fsz) {
        free(buf);
        fclose(fp);
        return NULL;
    }
    buf[fsz] = '\0';
    fclose(fp);

    return buf;
}

static int split_into(char *s, const char *delim, LfLines *out, int own_base, int strip_cr) {
    size_t n = 0;
    size_t cap = 8;
    char **lines = lf_reallocarray(NULL, cap, sizeof(char *));
    if (lines == NULL) {
        return -1;
    }

    char *t = strtok(s, delim);
    while (t != NULL) {
        if (strip_cr) {
            size_t len = strlen(t);
            if (len > 0 && t[len - 1] == '\r') {
                t[len - 1] = '\0';
            }
        }
        if (n == cap) {
            if (cap > SIZE_MAX / 2) {
                free(lines);
                return -1;
            }
            size_t ncap = cap * 2;
            char **tmp = lf_reallocarray(lines, ncap, sizeof(char *));
            if (tmp == NULL) {
                free(lines);
                return -1;
            }
            lines = tmp;
            cap = ncap;
        }
        lines[n++] = t;
        t = strtok(NULL, delim);
    }

    if (n == 0) {
        free(lines);
        lines = NULL;
    }

    out->base = own_base ? s : NULL;
    out->lines = lines;
    out->n = n;
    return 0;
}

int inp_split(char *s, const char *delim, LfLines *out) {
    if (s == NULL || delim == NULL || out == NULL) {
        return -1;
    }
    return split_into(s, delim, out, 0, 0);
}

int inp_get_lines(const char *path, LfLines *out) {
    if (path == NULL || out == NULL) {
        return -1;
    }

    char *s = inp_get_input(path);
    if (s == NULL) {
        return -1;
    }

    if (split_into(s, "\n", out, 1, 1) != 0) {
        free(s);
        return -1;
    }
    return 0;
}

int *inp_get_ints(const char *path, size_t *sz) {
    if (path == NULL || sz == NULL) {
        return NULL;
    }

    LfLines lines;
    if (inp_get_lines(path, &lines) != 0) {
        return NULL;
    }

    int *i = lf_reallocarray(NULL, lines.n == 0 ? 1 : lines.n, sizeof(int));
    if (i == NULL) {
        inp_lines_free(&lines);
        return NULL;
    }

    for (size_t idx = 0; idx < lines.n; idx++) {
        const char *errstr;
        int n = (int)strtonum(lines.lines[idx], INT_MIN, INT_MAX, &errstr);
        if (errstr) {
            free(i);
            inp_lines_free(&lines);
            return NULL;
        }
        i[idx] = n;
    }

    *sz = lines.n;
    inp_lines_free(&lines);
    return i;
}

void inp_lines_free(LfLines *lines) {
    if (lines == NULL) {
        return;
    }
    free(lines->lines);
    free(lines->base);
    lines->lines = NULL;
    lines->base = NULL;
    lines->n = 0;
}
