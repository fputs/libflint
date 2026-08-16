#ifndef LIBFLINT_INPUT_H
#define LIBFLINT_INPUT_H

#include <stddef.h>

/*
 * One-shot slurp of a regular, seekable file. fseek/ftell is required.
 * Lines from inp_get_lines have a trailing '\r' stripped so \r\n and \n
 * both work. inp_split requires a mutable buffer (strtok writes NULs).
 */

typedef struct {
    char *base;
    char **lines;
    size_t n;
} LfLines;

unsigned char *inp_get_binary(const char *path, size_t *fsz);

char *inp_get_input(const char *path);

int inp_split(char *s, const char *delim, LfLines *out);

int inp_get_lines(const char *path, LfLines *out);

int *inp_get_ints(const char *path, size_t *sz);

void inp_lines_free(LfLines *lines);

#endif /* LIBFLINT_INPUT_H */
