#ifndef LIBFLINT_H_STRING
#define LIBFLINT_H_STRING

#include <stddef.h>

#include "lfmemory.h"

/*
 * Counted string view. Does not own p. Predicates that do not
 * allocate take LfStr; the lfstr_* C-string wrappers are conveniences.
 */
typedef struct {
    const char *p;
    size_t n;
} LfStr;

LfStr lfstr_from(const char *s);

LfStr lfstr_trim_view(LfStr s);

int lfstr_starts_with_view(LfStr s, LfStr prefix);

int lfstr_ends_with_view(LfStr s, LfStr suffix);

int lfstr_contains_view(LfStr s, LfStr needle);

int lfstr_find(const char *haystack, const char *needle,
               size_t *num_substrings, size_t **substrings);

int lfstr_find_arena(ArenaAllocator *arena, const char *haystack,
                     const char *needle, size_t *num_substrings,
                     size_t **substrings);

char *lfstr_substr(const char *str, size_t idx, size_t len);

char *lfstr_substr_arena(ArenaAllocator *arena, const char *str,
                         size_t idx, size_t len);

char *lfstr_trim(const char *str);

char *lfstr_trim_arena(ArenaAllocator *arena, const char *str);

char *lfstr_join(const char **strs, size_t count, const char *sep);

char *lfstr_join_arena(ArenaAllocator *arena, const char **strs,
                       size_t count, const char *sep);

int lfstr_starts_with(const char *str, const char *prefix);

int lfstr_ends_with(const char *str, const char *suffix);

char *lfstr_replace(const char *str, const char *old, const char *replacement);

char *lfstr_replace_arena(ArenaAllocator *arena, const char *str,
                          const char *old, const char *replacement);

char *lfstr_to_upper(const char *str);

char *lfstr_to_upper_arena(ArenaAllocator *arena, const char *str);

char *lfstr_to_lower(const char *str);

char *lfstr_to_lower_arena(ArenaAllocator *arena, const char *str);

char **lfstr_split(const char *str, const char *delim, size_t *count);

char **lfstr_split_arena(ArenaAllocator *arena, const char *str,
                         const char *delim, size_t *count);

void lfstr_split_free(char **parts, size_t count);

int lfstr_contains(const char *str, const char *substr);

#endif /* LIBFLINT_H_STRING */
