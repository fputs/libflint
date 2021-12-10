#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef __linux__
#include <bsd/stdlib.h>
#endif

#include "input.h"

char *get_input(const char *path) {
    FILE *fp = NULL;
    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open %s. Returning NULL\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t fsz = ftell(fp);
    rewind(fp);

    char* buf = NULL;
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
        n = strtonum(lines[idx], INT_MIN, INT_MAX, &errstr);
        if (errstr) {
            printf("Failed to convert %s to int. Returning NULL\n", lines[idx]);
            exit(1);
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
