#ifndef LIBFLINT_INPUT_H
#define LIBFLINT_INPUT_H

#include <stdlib.h>

char *get_binary(const char *);

char *get_input(const char *);

char **split(char *, size_t *, const char *);

char **get_lines(const char *, size_t *);

int *get_ints(const char *, size_t *);

void del_split(char **);

void del_lines(char **);

#endif // LIBFLINT_INPUT_H
