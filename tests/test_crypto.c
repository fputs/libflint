#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfcrypto.h"

int main() {
    const unsigned char *in = (const unsigned char *)"BUTT";
    char *cs = b64_encode(in, strlen((const char *)in));
    ASSERT_STR_EQ(cs, "QlVUVA==");
    free(cs);

    const unsigned char *in2 = (const unsigned char *)"a longer base64 test, apparently";
    cs = b64_encode(in2, strlen((const char *)in2));
    ASSERT_STR_EQ(cs, "YSBsb25nZXIgYmFzZTY0IHRlc3QsIGFwcGFyZW50bHk=");
    free(cs);

    char *out2 = "YSBsb25nZXIgYmFzZTY0IHRlc3QsIGFwcGFyZW50bHk=";
    size_t s_sz = 0;
    unsigned char *us = b64_decode(out2, strlen(out2), &s_sz);
    ASSERT_STR_EQ((char *)us, "a longer base64 test, apparently");
    ASSERT_EQ(strlen((char *)us), s_sz);
    free(us);

    us = hex_decode("DEADBEEF", &s_sz);
    unsigned char h[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
    for (size_t i = 0; i < 4; ++i) {
        ASSERT_EQ(us[i], h[i]);
    }
    free(us);

    us = hex_decode("f00f5", &s_sz);
    unsigned char h2[3] = { 0x0F, 0x00, 0xF5 };
    for (size_t i = 0; i < 3; ++i) {
        ASSERT_EQ(us[i], h2[i]);
    }
    free(us);

    us = hex_decode("0xf00f5", &s_sz);
    for (size_t i = 0; i < 3; ++i) {
        ASSERT_EQ(us[i], h2[i]);
    }
    free(us);

    char *enc_s = hex_encode(h, 4);
    ASSERT_STR_EQ(enc_s, "deadbeef");
    free(enc_s);

    unsigned char hexsup[4] = { 0x53, 0x75, 0x70, 0x3F };
    enc_s = hex_to_str(hexsup, 4);
    ASSERT_STR_EQ(enc_s, "Sup?");
    free(enc_s);

    unsigned char *xor_s = repeating_key_xor_s("TEST", "HI");
    enc_s = hex_encode(xor_s, 4);
    ASSERT_STR_EQ(enc_s, "1c0c1b1d");
    free(enc_s);
    free(xor_s);

    unsigned char ua[2] = {0x2, 0xF};
    unsigned char ub[2] = {0x4, 0xE};
    int hamming = hamming_distance(ua, ub, 2);
    ASSERT_EQ(hamming, 3);

    hamming = hamming_distance_s("this is a test", "wokka wokka!!!");
    ASSERT_EQ(hamming, 37);

    /* -- expanded tests -- */

    /* empty b64 encode/decode */
    cs = b64_encode((const unsigned char *)"", 0);
    ASSERT_STR_EQ(cs, "");
    free(cs);

    us = b64_decode("", 0, &s_sz);
    ASSERT_EQ(s_sz, (size_t)0);
    free(us);

    /* empty hex encode */
    enc_s = hex_encode((const unsigned char *)"", 0);
    ASSERT_STR_EQ(enc_s, "");
    free(enc_s);

    /* sz * 2 + 1 wrap must fail closed */
    enc_s = hex_encode((const unsigned char *)"x", (SIZE_MAX - 1) / 2 + 1);
    ASSERT_NULL(enc_s);
    enc_s = hex_encode(NULL, 4);
    ASSERT_NULL(enc_s);

    /* hex_to_str empty */
    enc_s = hex_to_str((const unsigned char *)"", 0);
    ASSERT_STR_EQ(enc_s, "");
    free(enc_s);

    /* binary roundtrip through hex encode/decode */
    unsigned char bin[5] = { 0x00, 0xFF, 0x7F, 0x80, 0x01 };
    enc_s = hex_encode(bin, 5);
    us = hex_decode(enc_s, &s_sz);
    ASSERT_EQ(s_sz, (size_t)5);
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(us[i], bin[i]);
    }
    free(enc_s);
    free(us);

    /* hex_decode with NULL sz returns NULL */
    us = hex_decode("DEADBEEF", NULL);
    ASSERT_NULL(us);

    /* hex_decode size validation */
    us = hex_decode("AB", &s_sz);
    ASSERT_EQ(s_sz, (size_t)1);
    ASSERT_EQ(us[0], (unsigned char)0xAB);
    free(us);

    us = hex_decode("ABCD", &s_sz);
    ASSERT_EQ(s_sz, (size_t)2);
    free(us);

    /* hamming_distance_s mismatched lengths returns -1 */
    hamming = hamming_distance_s("short", "longer string");
    ASSERT_EQ(hamming, -1);

    /* hamming_distance empty strings */
    hamming = hamming_distance_s("", "");
    ASSERT_EQ(hamming, 0);

    /* hamming_distance identical strings */
    hamming = hamming_distance_s("same", "same");
    ASSERT_EQ(hamming, 0);

    /* repeating_key_xor with zero-length key returns NULL */
    xor_s = repeating_key_xor((const unsigned char *)"test", 4, (const unsigned char *)"", 0);
    ASSERT_NULL(xor_s);

    TEST_REPORT();
}
