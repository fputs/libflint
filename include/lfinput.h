#ifndef LIBFLINT_INPUT_H
#define LIBFLINT_INPUT_H

#include <stdlib.h>

unsigned char *inp_get_binary(const char *, size_t *fsz);

char *inp_get_input(const char *);

char **inp_split(char *, size_t *, const char *);

char **inp_get_lines(const char *, size_t *);

int *inp_get_ints(const char *, size_t *);

void inp_del_split(char **);

void inp_del_lines(char **);

#define DEFAULT_CAPTURE_SYSTEM_BUFSIZE 1024

char *inp_capture_system(const char *cmd, size_t buf_sz);

#endif // LIBFLINT_INPUT_H
