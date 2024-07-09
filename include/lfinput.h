#ifndef LIBFLINT_INPUT_H
#define LIBFLINT_INPUT_H

#include <stdlib.h>

unsigned char *get_binary(const char *, size_t *fsz);

char *get_input(const char *);

char **split(char *, size_t *, const char *);

char **get_lines(const char *, size_t *);

int *get_ints(const char *, size_t *);

void del_split(char **);

void del_lines(char **);

#define DEFAULT_CAPTURE_SYSTEM_BUFSIZE 1024

const char *capture_system(const char *cmd, int buf_sz);

#endif // LIBFLINT_INPUT_H
