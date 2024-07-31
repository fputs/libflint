#ifndef LIBFLINT_CRYPTO_H
#define LIBFLINT_CRYPTO_H

#include <stddef.h>

char *b64_encode(const unsigned char *s, size_t sz);
unsigned char *b64_decode(const char *s, size_t sz, size_t *decode_sz);

char *hex_encode(const unsigned char *hex, size_t sz);
unsigned char *hex_decode(const char *orig, size_t *sz);
char *hex_to_str(const unsigned char *hex, size_t sz);

unsigned char* repeating_key_xor(const unsigned char* s, size_t s_sz, const unsigned char* key, size_t k_sz);
unsigned char *repeating_key_xor_s(const char* s, const char* key);

unsigned int hamming_distance_s(const char *a, const char *b);
unsigned int hamming_distance(unsigned char *a, unsigned char *b, size_t sz);

#endif // LIBFLINT_CRYPTO_H
