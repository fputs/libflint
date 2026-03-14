#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#ifdef __linux__

#include <bsd/stdlib.h>

#endif

#include "lfinput.h"

static FILE* inp_open_file(const char *path, size_t *fsz, const char *mode) {
    FILE *fp = NULL;
    fp = fopen(path, mode);
    if (fp == NULL) {
        fprintf(stderr, "Failed to open %s. Returning NULL\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long pos = ftell(fp);
    if (pos < 0) {
        fprintf(stderr, "ftell failed for %s. Returning NULL\n", path);
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

    unsigned char *buf = NULL;
    buf = (unsigned char*)malloc(sizeof(unsigned char) * (*fsz));
    if (buf == NULL) {
        fprintf(stderr, "Failed to malloc buf. Returning NULL\n");
        fclose(fp);
        return NULL;
    }

    fread(buf, 1, *fsz, fp);
    fclose(fp);

    return buf;
}

char *inp_get_input(const char *path) {
    size_t fsz = 0;
    FILE *fp = inp_open_file(path, &fsz, "r");
    if (fp == NULL) {
        return NULL;
    }

    char *buf = NULL;
    buf = malloc(fsz + 1);
    if (buf == NULL) {
        fprintf(stderr, "Failed to malloc buf. Returning NULL\n");
        fclose(fp);
        return NULL;
    }

    fread(buf, 1, fsz, fp);
    buf[fsz] = '\0';
    fclose(fp);

    return buf;
}

char **inp_split(char *s, size_t *lsz, const char *delim) {
    if (s == NULL || lsz == NULL || delim == NULL) {
        return NULL;
    }

    char **lines = NULL;
    char *t = strtok(s, delim);
    size_t n = 0;

    while (t != NULL) {
        char **tmp = realloc(lines, sizeof(char *) * (n + 1));
        if (tmp == NULL) {
            fprintf(stderr, "Failed to realloc lines buffer. Returning NULL\n");
            free(lines);
            return NULL;
        }
        lines = tmp;
        n++;
        lines[n - 1] = t;
        t = strtok(NULL, delim);
    }

    *lsz = n;
    return lines;
}

/*
 * Internal split used by inp_get_lines. Stores the original string pointer
 * at index [-1] (one slot before the returned array) so inp_del_lines can
 * free the correct base even when strtok skips leading delimiters.
 */
static char **split_lines(char *s, size_t *lsz, const char *delim) {
    if (s == NULL || lsz == NULL || delim == NULL) {
        return NULL;
    }

    char *base = s;
    /* Start with space for the hidden base slot + 1 token */
    size_t n = 0;
    size_t cap = 2;
    char **raw = malloc(sizeof(char *) * cap);
    if (raw == NULL) {
        free(base);
        return NULL;
    }
    raw[0] = base;  /* hidden slot stores original allocation */

    char *t = strtok(s, delim);
    while (t != NULL) {
        n++;
        if (n + 1 >= cap) {
            cap *= 2;
            char **tmp = realloc(raw, sizeof(char *) * cap);
            if (tmp == NULL) {
                free(raw);
                free(base);
                *lsz = 0;
                return NULL;
            }
            raw = tmp;
        }
        raw[n] = t;
        t = strtok(NULL, delim);
    }

    if (n == 0) {
        free(base);
        free(raw);
        *lsz = 0;
        return NULL;
    }

    *lsz = n;
    return raw + 1;  /* return pointer past the hidden base slot */
}

char **inp_get_lines(const char *path, size_t *lsz) {
    return split_lines(inp_get_input(path), lsz, "\n");
}

int *inp_get_ints(const char *path, size_t *sz) {
    if (path == NULL || sz == NULL) {
        return NULL;
    }

    char **lines = inp_get_lines(path, sz);
    if (lines == NULL) {
        return NULL;
    }

    int *i = malloc(sizeof(int) * *sz);
    if (i == NULL) {
        inp_del_lines(lines);
        return NULL;
    }

    for (size_t idx = 0; idx < *sz; idx++) {
        int n;
        const char *errstr;
        n = (int) strtonum(lines[idx], INT_MIN, INT_MAX, &errstr);
        if (errstr) {
            fprintf(stderr, "Failed to convert %s to int. Returning NULL\n", lines[idx]);
            free(i);
            inp_del_lines(lines);
            return NULL;
        }
        i[idx] = n;
    }

    inp_del_lines(lines);
    return i;
}

void inp_del_split(char **sp) {
    if (sp == NULL) {
        return;
    }
    free(sp);
}

void inp_del_lines(char **lines) {
    if (lines == NULL) {
        return;
    }
    /* lines[-1] holds the original string base pointer (set by split_lines) */
    free(lines[-1]);
    free(lines - 1);
}

char *inp_capture_system(const char *cmd, size_t buf_sz) {
    if (cmd == NULL) {
        return NULL;
    }

    if (buf_sz == 0) {
        buf_sz = DEFAULT_CAPTURE_SYSTEM_BUFSIZE;
    }

    FILE *tmp = popen(cmd, "r");
    if (tmp == NULL) {
        fprintf(stderr, "libflint: failed to open FILE *tmp in capture_system. Errno: %d\n", errno);
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
            cap *= 2;
            char *tmp2 = realloc(buf, cap);
            if (tmp2 == NULL) {
                free(buf);
                pclose(tmp);
                return NULL;
            }
            buf = tmp2;
        }
    }

    buf[total] = '\0';
    pclose(tmp);
    return buf;
}
