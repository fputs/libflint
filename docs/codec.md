Hex and Base64 encode and decode. These are codecs, not cryptographic primitives.

## Usage

Encode bytes with `hex_encode` or `b64_encode`. The allocating helpers return `NULL` on error. Free a successful result with `free()`.

```c
char *encoded = b64_encode((const unsigned char *)"Hello", 5);
if (encoded != NULL) {
    printf("%s\n", encoded); /* Prints "SGVsbG8=" */
    free(encoded);
}
```

Use the `*_into` functions to write into your own buffer. They return `0` on success or `-1` on error. Decoders write the output size only on success, but may modify the destination before returning an error. Discard that buffer's contents on failure.

Hex strings are lowercase with no leading `0x`. `hex_decode` / `hex_decode_into` accept optional `0x` or `0X`.

## Functions

### b64_encode / b64_encode_into

Encodes bytes as a NUL-terminated Base64 string. `dest_cap` must be at least `4 * ceil(sz / 3) + 1` (NUL included). A NULL source is accepted only when `sz == 0`; the destination must be non-NULL.

```c
char *b64_encode(const unsigned char *s, size_t sz);
int b64_encode_into(const unsigned char *s, size_t sz, char *dest, size_t dest_cap);
```


### b64_decode / b64_decode_into

Decodes standard Base64, with or without trailing padding. An unpadded final group of two or three characters is accepted; a one-character group is rejected. Invalid characters (including whitespace), misplaced padding, and incomplete padding return an error. Unused bits in the final group are not checked.

The allocating form is NUL-terminated but may contain interior NULs, so use `decode_sz`, not `strlen`. The `*_into` form needs room for the decoded bytes and adds a NUL only if capacity remains. `decode_sz` is optional and is written only on success. A NULL source is accepted only when `sz == 0`; the destination must be non-NULL.

```c
unsigned char *b64_decode(const char *s, size_t sz, size_t *decode_sz);
int b64_decode_into(const char *s, size_t sz, unsigned char *dest, size_t dest_cap,
                    size_t *decode_sz);

/* Usage */
size_t out_sz;
unsigned char *decoded = b64_decode("SGVsbG8=", 8, &out_sz);
/* decoded: "Hello", out_sz: 5 */
free(decoded);
```

### hex_encode / hex_encode_into

`[0xDE, 0xAD, 0xBE, 0xEF]` becomes `"deadbeef"`. Returns `NULL` / `-1` if `sz * 2 + 1` would wrap `size_t` or `dest_cap` is smaller than that.

```c
char *hex_encode(const unsigned char *hex, size_t sz);
int hex_encode_into(const unsigned char *src, size_t n, char *dest, size_t dest_cap);
```

### hex_decode / hex_decode_into

Decodes a NUL-terminated hex string, accepting uppercase and lowercase digits. Odd-length input is padded with a leading nibble of `0`. Returns `NULL` / `-1` if any character is not a hex digit. The result is raw bytes without a NUL terminator. An empty string or bare `0x` prefix produces zero bytes.

`hex_decode` requires a non-NULL `sz`; `hex_decode_into` accepts a NULL `out_sz`. Output sizes are written only on success. The destination capacity must hold all decoded bytes.

```c
unsigned char *hex_decode(const char *orig, size_t *sz);
int hex_decode_into(const char *src, unsigned char *dest, size_t dest_cap, size_t *out_sz);
```

### hex_to_str

Copies `sz` bytes into a freshly allocated buffer and appends a NUL terminator. Interior NUL bytes are preserved. The caller frees the result. Returns `NULL` on allocation failure, if `sz == SIZE_MAX`, or if `hex` is NULL with a nonzero size.

```c
char *hex_to_str(const unsigned char *hex, size_t sz);
```
