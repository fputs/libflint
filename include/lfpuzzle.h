#ifndef LIBFLINT_PUZZLE_H
#define LIBFLINT_PUZZLE_H

#include <stddef.h>

/*
 * Cryptopals-style byte puzzles: repeating-key XOR, Hamming distance,
 * and a toy English-likelihood scorer. Not a cryptography library.
 * Do not use this to invent AES/SHA/RNG.
 */

unsigned char *repeating_key_xor(const unsigned char *s, size_t s_sz,
                                 const unsigned char *key, size_t k_sz);
unsigned char *repeating_key_xor_s(const char *s, const char *key);

/* Return -1 for invalid inputs or if the distance exceeds INT_MAX. */
int hamming_distance_s(const char *a, const char *b);
int hamming_distance(const unsigned char *a, const unsigned char *b, size_t sz);

/* ETAOIN SHRDLU score. Returns -1 for NULL or a score exceeding INT_MAX. */
int simple_english_scoring(const char *s);

#endif /* LIBFLINT_PUZZLE_H */
