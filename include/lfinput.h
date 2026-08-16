#ifndef LIBFLINT_INPUT_H
#define LIBFLINT_INPUT_H

#include <stddef.h>

#include "lfmemory.h"

/*
 * One-shot slurp of a regular, seekable file. fseek/ftell is required.
 * Lines from inp_get_lines have a trailing '\r' stripped so \r\n and \n
 * both work. Split is lfstr_split: string delimiter, empty tokens kept,
 * source is not mutated.
 *
 * *_arena variants allocate from the arena. Do not free those results
 * with free(); inp_lines_free on an arena-backed LfLines is a no-op
 * on the memory (heap == 0).
 */

typedef struct {
    char *base;
    char **lines;
    size_t n;
    int heap;
} LfLines;

unsigned char *inp_get_binary(const char *path, size_t *fsz);

unsigned char *inp_get_binary_arena(ArenaAllocator *arena, const char *path,
                                    size_t *fsz);

char *inp_get_input(const char *path);

char *inp_get_input_arena(ArenaAllocator *arena, const char *path);

int inp_split(const char *s, const char *delim, LfLines *out);

int inp_split_arena(ArenaAllocator *arena, const char *s, const char *delim,
                    LfLines *out);

int inp_get_lines(const char *path, LfLines *out);

int inp_get_lines_arena(ArenaAllocator *arena, const char *path, LfLines *out);

int *inp_get_ints(const char *path, size_t *sz);

int *inp_get_ints_arena(ArenaAllocator *arena, const char *path, size_t *sz);

void inp_lines_free(LfLines *lines);

#endif /* LIBFLINT_INPUT_H */
