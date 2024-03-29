#ifndef LIBFLINT_CRYPTO_H
#define LIBFLINT_CRYPTO_H

#include <stddef.h>

unsigned char *b64_encode(const unsigned char *s, size_t sz);
unsigned char *b64_decode(const char *s, size_t sz, size_t *decode_sz);
unsigned char *hex_decode(const char *orig, size_t *sz);

#endif // LIBFLINT_CRYPTO_H
