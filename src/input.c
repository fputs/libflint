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

static void *inp_alloc(ArenaAllocator *arena, size_t n) {
    if (arena != NULL) {
        return arena_malloc(arena, n);
    }
    return malloc(n);
}

static void inp_alloc_fail(ArenaAllocator *arena, void *p) {
    if (arena == NULL) {
        free(p);
    }
}

unsigned char *inp_get_binary_arena(ArenaAllocator *arena, const char *path,
                                    size_t *fsz) {
    FILE *fp = inp_open_file(path, fsz, "rb");
    if (fp == NULL) {
        return NULL;
    }

    unsigned char *buf = inp_alloc(arena, *fsz == 0 ? 1 : *fsz);
    if (buf == NULL) {
        fclose(fp);
        return NULL;
    }

    if (*fsz > 0 && fread(buf, 1, *fsz, fp) != *fsz) {
        inp_alloc_fail(arena, buf);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    return buf;
}

unsigned char *inp_get_binary(const char *path, size_t *fsz) {
    return inp_get_binary_arena(NULL, path, fsz);
}

char *inp_get_input_arena(ArenaAllocator *arena, const char *path) {
    size_t fsz = 0;
    FILE *fp = inp_open_file(path, &fsz, "r");
    if (fp == NULL) {
        return NULL;
    }

    if (fsz == SIZE_MAX) {
        fclose(fp);
        return NULL;
    }

    char *buf = inp_alloc(arena, fsz + 1);
    if (buf == NULL) {
        fclose(fp);
        return NULL;
    }

    if (fsz > 0 && fread(buf, 1, fsz, fp) != fsz) {
        inp_alloc_fail(arena, buf);
        fclose(fp);
        return NULL;
    }
    buf[fsz] = '\0';
    fclose(fp);

    return buf;
}

char *inp_get_input(const char *path) {
    return inp_get_input_arena(NULL, path);
}

static int split_into(char *s, const char *delim, LfLines *out, int own_base,
                      int strip_cr, ArenaAllocator *arena) {
    size_t n = 0;
    size_t cap = 8;
    char **lines;
    if (arena != NULL) {
        lines = arena_malloc(arena, cap * sizeof(char *));
    } else {
        lines = lf_reallocarray(NULL, cap, sizeof(char *));
    }
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
                inp_alloc_fail(arena, lines);
                return -1;
            }
            size_t ncap = cap * 2;
            char **tmp;
            if (arena != NULL) {
                tmp = arena_malloc(arena, ncap * sizeof(char *));
                if (tmp == NULL) {
                    return -1;
                }
                memcpy(tmp, lines, n * sizeof(char *));
            } else {
                tmp = lf_reallocarray(lines, ncap, sizeof(char *));
                if (tmp == NULL) {
                    free(lines);
                    return -1;
                }
            }
            lines = tmp;
            cap = ncap;
        }
        lines[n++] = t;
        t = strtok(NULL, delim);
    }

    if (n == 0) {
        inp_alloc_fail(arena, lines);
        lines = NULL;
    }

    out->base = own_base ? s : NULL;
    out->lines = lines;
    out->n = n;
    out->heap = (arena == NULL);
    return 0;
}

int inp_split(char *s, const char *delim, LfLines *out) {
    if (s == NULL || delim == NULL || out == NULL) {
        return -1;
    }
    return split_into(s, delim, out, 0, 0, NULL);
}

int inp_get_lines_arena(ArenaAllocator *arena, const char *path, LfLines *out) {
    if (path == NULL || out == NULL) {
        return -1;
    }

    char *s = inp_get_input_arena(arena, path);
    if (s == NULL) {
        return -1;
    }

    if (split_into(s, "\n", out, 1, 1, arena) != 0) {
        inp_alloc_fail(arena, s);
        return -1;
    }
    return 0;
}

int inp_get_lines(const char *path, LfLines *out) {
    return inp_get_lines_arena(NULL, path, out);
}

int *inp_get_ints_arena(ArenaAllocator *arena, const char *path, size_t *sz) {
    if (path == NULL || sz == NULL) {
        return NULL;
    }

    LfLines lines;
    if (inp_get_lines_arena(arena, path, &lines) != 0) {
        return NULL;
    }

    size_t n = lines.n == 0 ? 1 : lines.n;
    int *i;
    if (arena != NULL) {
        if (n > SIZE_MAX / sizeof(int)) {
            return NULL;
        }
        i = arena_malloc(arena, n * sizeof(int));
    } else {
        i = lf_reallocarray(NULL, n, sizeof(int));
    }
    if (i == NULL) {
        inp_lines_free(&lines);
        return NULL;
    }

    for (size_t idx = 0; idx < lines.n; idx++) {
        const char *errstr;
        int val = (int)strtonum(lines.lines[idx], INT_MIN, INT_MAX, &errstr);
        if (errstr) {
            inp_alloc_fail(arena, i);
            inp_lines_free(&lines);
            return NULL;
        }
        i[idx] = val;
    }

    *sz = lines.n;
    inp_lines_free(&lines);
    return i;
}

int *inp_get_ints(const char *path, size_t *sz) {
    return inp_get_ints_arena(NULL, path, sz);
}

void inp_lines_free(LfLines *lines) {
    if (lines == NULL) {
        return;
    }
    if (lines->heap) {
        free(lines->lines);
        free(lines->base);
    }
    lines->lines = NULL;
    lines->base = NULL;
    lines->n = 0;
    lines->heap = 0;
}
