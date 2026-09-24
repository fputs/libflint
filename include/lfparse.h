#ifndef LIBFLINT_H_PARSE
#define LIBFLINT_H_PARSE
#include <stdint.h>
#include "lfstring.h"

typedef enum {
    LF_PARSE_OK = 0,
    LF_PARSE_INVALID = -1,
    LF_PARSE_OVERFLOW = -2
} LfParseStatus;

/* Bases 2..36, no prefixes or implicit whitespace skipping. Consume a digit
 * prefix on success; input and output remain unchanged on any failure. */
LfParseStatus lf_parse_u64(LfStr *input, unsigned base, uint64_t *value);
LfParseStatus lf_parse_i64(LfStr *input, unsigned base, int64_t *value);
/* Consume an exact prefix delimiter, including an empty delimiter. */
LfParseStatus lf_parse_consume(LfStr *input, LfStr delimiter);
/* Skip ASCII space, tab, newline, carriage return, form feed, vertical tab. */
size_t lf_parse_skip_whitespace(LfStr *input);
/* Extract a nonempty borrowed token up to any byte in delimiters.
 * Leave the delimiter unconsumed; do not skip leading delimiters. */
LfParseStatus lf_parse_token(LfStr *input, LfStr delimiters, LfStr *token);
#endif
