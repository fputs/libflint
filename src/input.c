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
#include "lfstring.h"

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

static void strip_trailing_cr(char **parts, size_t n) {
    for (size_t i = 0; i < n; i++) {
        size_t len = strlen(parts[i]);
        if (len > 0 && parts[i][len - 1] == '\r') {
            parts[i][len - 1] = '\0';
        }
    }
}

int inp_split_arena(ArenaAllocator *arena, const char *s, const char *delim,
                    LfLines *out) {
    if (s == NULL || delim == NULL || out == NULL) {
        return -1;
    }

    size_t n = 0;
    char **parts = lfstr_split_arena(arena, s, delim, &n);
    if (parts == NULL) {
        return -1;
    }

    out->base = NULL;
    out->lines = parts;
    out->n = n;
    out->heap = (arena == NULL);
    return 0;
}

int inp_split(const char *s, const char *delim, LfLines *out) {
    return inp_split_arena(NULL, s, delim, out);
}

int inp_get_lines_arena(ArenaAllocator *arena, const char *path, LfLines *out) {
    if (path == NULL || out == NULL) {
        return -1;
    }

    char *s = inp_get_input_arena(arena, path);
    if (s == NULL) {
        return -1;
    }

    if (s[0] == '\0') {
        inp_alloc_fail(arena, s);
        out->base = NULL;
        out->lines = NULL;
        out->n = 0;
        out->heap = (arena == NULL);
        return 0;
    }

    size_t slen = strlen(s);
    size_t n = 0;
    char **parts = lfstr_split_arena(arena, s, "\n", &n);
    if (parts == NULL) {
        inp_alloc_fail(arena, s);
        return -1;
    }

    int drop_last = (n > 0 && parts[n - 1][0] == '\0' && s[slen - 1] == '\n');
    inp_alloc_fail(arena, s);
    strip_trailing_cr(parts, n);

    if (drop_last) {
        if (arena == NULL) {
            free(parts[n - 1]);
        }
        n--;
        if (n == 0) {
            if (arena == NULL) {
                free(parts);
            }
            parts = NULL;
        }
    }

    out->base = NULL;
    out->lines = parts;
    out->n = n;
    out->heap = (arena == NULL);
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
        lfstr_split_free(lines->lines, lines->n);
        free(lines->base);
    }
    lines->lines = NULL;
    lines->base = NULL;
    lines->n = 0;
    lines->heap = 0;
}
