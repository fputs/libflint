Parse integers and consume tokens from borrowed `LfStr` views without temporary
NUL-terminated copies. Include `lfparse.h`. Complements [[string]] and [[input]].
The caller owns the views and underlying text; this module never allocates or
modifies the text. Successful operations advance the input view, leaving the
remaining input available to the caller.

## Usage

Create a view over the input, skip any leading whitespace, and parse a number. The view advances only when parsing succeeds.

```c
LfStr input = lfstr_from("  -42,tail");
lf_parse_skip_whitespace(&input);
int64_t value;
if (lf_parse_i64(&input, 10, &value) == LF_PARSE_OK) {
    /* value == -42; input is now ",tail" */
    lf_parse_consume(&input, lfstr_from(","));
}
```

## Types

### LfParseStatus

```c
typedef enum {
    LF_PARSE_OK = 0,
    LF_PARSE_INVALID = -1,
    LF_PARSE_OVERFLOW = -2
} LfParseStatus;
```

`LfStr` is defined in `lfstring.h`. An `LfStr` may have a NULL data pointer only when its length is zero.
The `LfStr *input` handle itself must be non-NULL. Failed status-returning operations leave input and output unchanged.
Outputs must not alias input handles or underlying text. Token views borrow the
original storage, which must remain alive while they are used.

## Functions

### lf_parse_u64 / lf_parse_i64

Parse the longest initial digit sequence in an explicit base from 2 through 36.
Digits are ASCII `0`–`9`, `a`–`z`, or `A`–`Z`. Both accept an optional `+`; signed
parsing also accepts `-`. Unsigned parsing rejects any minus sign, including `-0`.
At least one valid digit is required after a sign. No whitespace is skipped.

Return `LF_PARSE_OK` on success, `LF_PARSE_INVALID` for invalid arguments or no
initial digits, and `LF_PARSE_OVERFLOW` when the numeric prefix exceeds the target
range. Signed endpoints are `INT64_MIN` and `INT64_MAX`; unsigned values extend
through `UINT64_MAX`. Overflow consumes nothing, including any sign.

A non-digit or digit outside the selected base ends a successful prefix. There
is no automatic base detection or prefix recognition: parsing `0xff` in base 16
produces zero and leaves `xff`. Consume `0x` explicitly first if needed. Require
an empty remaining view (or a known delimiter) when a complete number is needed.
Embedded NUL is an ordinary non-digit, not an end-of-view marker.

```c
LfParseStatus lf_parse_u64(LfStr *input, unsigned base, uint64_t *value);
LfParseStatus lf_parse_i64(LfStr *input, unsigned base, int64_t *value);
```

### lf_parse_consume

Consume an exact byte prefix. Return `LF_PARSE_OK` for a match,
`LF_PARSE_INVALID` for a mismatch or invalid arguments. An empty delimiter always
matches a valid input and consumes nothing. Multi-byte and embedded-NUL delimiters
are supported.

```c
LfParseStatus lf_parse_consume(LfStr *input, LfStr delimiter);
```

### lf_parse_skip_whitespace

Consume ASCII space, tab, newline, carriage return, form feed, and vertical tab.
Return the number of bytes consumed. Invalid/NULL input returns zero. Independent
of locale; high-bit bytes and embedded NUL are not whitespace.

```c
size_t lf_parse_skip_whitespace(LfStr *input);
```

### lf_parse_token

Extract a nonempty token ending at any byte from the delimiter view or the end
of input. Leave the delimiter unconsumed. Leading delimiters and empty input
return `LF_PARSE_INVALID`; they are not skipped. An empty delimiter set takes
all remaining input. Return `LF_PARSE_OK` on success; `token` and `input` must
be distinct, non-NULL pointers. Delimiters are bytes, not Unicode characters.

```c
LfParseStatus lf_parse_token(LfStr *input, LfStr delimiters, LfStr *token);

/* Usage */
LfStr input = lfstr_from("name=value");
LfStr key;
if (lf_parse_token(&input, lfstr_from("="), &key) == LF_PARSE_OK) {
    /* key is "name", input is "=value" */
    lf_parse_consume(&input, lfstr_from("="));
}
```
