#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef __linux__

#include <bsd/stdlib.h>

#endif

#include "input.h"

static FILE* open_file(const char *path, size_t *fsz) {
    FILE *fp = NULL;
    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open %s. Returning NULL\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *fsz = ftell(fp);
    rewind(fp);

    return fp;
}

char *get_binary(const char *path, size_t *fsz) {
    FILE *fp = open_file(path, fsz);
    if (fp == NULL) {
        return NULL;
    }

    char *buf = NULL;
    buf = malloc(*fsz);
    if (buf == NULL) {
        fprintf(stderr, "Failed to malloc buf. Returning NULL\n");
        fclose(fp);
        return NULL;
    }

    fread(buf, 1, *fsz, fp);
    fclose(fp);

    return buf;
}

char *get_input(const char *path) {
    size_t fsz = 0;
    FILE *fp = open_file(path, &fsz);
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

char **split(char *s, size_t *lsz, const char *delim) {
    char **lines = NULL;
    char *t = strtok(s, delim);
    size_t n = 0;

    while (t != NULL) {
        lines = realloc(lines, sizeof(char *) * ++n);
        if (lines == NULL) {
            fprintf(stderr, "Failed to realloc lines buffer. Returning NULL\n");
            free(s);
            return NULL;
        }
        lines[n - 1] = t;
        t = strtok(NULL, delim);
    }

    *lsz = n;
    return lines;
}

char **get_lines(const char *path, size_t *lsz) {
    return split(get_input(path), lsz, "\n");
}

int *get_ints(const char *path, size_t *sz) {
    char **lines = get_lines(path, sz);
    int *i = malloc(sizeof(int) * *sz);

    for (size_t idx = 0; idx < *sz; idx++) {
        int n;
        const char *errstr;
        n = (int) strtonum(lines[idx], INT_MIN, INT_MAX, &errstr);
        if (errstr) {
            fprintf(stderr, "Failed to convert %s to int. Returning NULL\n", lines[idx]);
            free(i);
            del_lines(lines);
            return NULL;
        }
        i[idx] = n;
    }

    del_lines(lines);
    return i;
}

void del_split(char **sp) {
    free(sp[0]);
    free(sp);
}

void del_lines(char **lines) {
    del_split(lines);
}
