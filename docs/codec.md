Hex and Base64 encode and decode. These are codecs, not cryptographic primitives.

Allocating helpers return a heap buffer the caller must free. `*_into` writes into a caller-owned buffer and returns `0` on success or `-1` on error (NULL arguments, size wrap, or `dest_cap` too small). Decode is fail-closed: invalid input returns an error and does not publish an output size.

Hex strings are lowercase with no leading `0x`. `hex_decode` / `hex_decode_into` accept optional `0x` or `0X`.

## Functions

### b64_encode / b64_encode_into

```c
char *b64_encode(const unsigned char *s, size_t sz);
int b64_encode_into(const unsigned char *s, size_t sz, char *dest, size_t dest_cap);

/* Usage */
char *encoded = b64_encode((unsigned char *)"Hello", 5);
/* encoded: "SGVsbG8=" */
free(encoded);

char buf[16];
b64_encode_into((unsigned char *)"Hello", 5, buf, sizeof buf);
```

`dest_cap` must be at least `4 * ceil(sz / 3) + 1` (NUL included).

### b64_decode / b64_decode_into

Invalid characters, truncated input, and incomplete padding return an error. A valid prefix of a corrupt string is not returned. The allocating form is NUL-terminated but may contain interior NULs, so use `decode_sz`, not `strlen`. `*decode_sz` is written only on success.

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

Odd-length input is padded with a leading nibble of `0`. Returns `NULL` / `-1` if any character is not a hex digit. `*sz` / `*out_sz` is written only on success.

```c
unsigned char *hex_decode(const char *orig, size_t *sz);
int hex_decode_into(const char *src, unsigned char *dest, size_t dest_cap, size_t *out_sz);
```

### hex_to_str

Copies bytes into a freshly allocated C string. Not hex encoding.

```c
char *hex_to_str(const unsigned char *hex, size_t sz);
```
