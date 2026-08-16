#ifndef LIBFLINT_CODEC_H
#define LIBFLINT_CODEC_H

#include <stddef.h>

/*
 * Hex and Base64 codecs. Not cryptography.
 * Allocating helpers return a heap buffer the caller frees.
 * *_into writes into a caller-sized buffer (0 success, -1 error).
 */

char *b64_encode(const unsigned char *s, size_t sz);
unsigned char *b64_decode(const char *s, size_t sz, size_t *decode_sz);
int b64_encode_into(const unsigned char *s, size_t sz, char *dest, size_t dest_cap);
int b64_decode_into(const char *s, size_t sz, unsigned char *dest, size_t dest_cap,
                    size_t *decode_sz);

char *hex_encode(const unsigned char *hex, size_t sz);
unsigned char *hex_decode(const char *orig, size_t *sz);
int hex_encode_into(const unsigned char *src, size_t n, char *dest, size_t dest_cap);
int hex_decode_into(const char *src, unsigned char *dest, size_t dest_cap, size_t *out_sz);
char *hex_to_str(const unsigned char *hex, size_t sz);

#endif /* LIBFLINT_CODEC_H */
