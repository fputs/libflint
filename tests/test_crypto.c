#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfcrypto.h"

int main() {
    char *in = "BUTT";
    unsigned char *s = b64_encode(in, strlen(in));
    ASSERT_STR_EQ((char *)s, "QlVUVA==");
    free(s);

    char *in2 = "a longer base64 test, apparently";
    s = b64_encode(in2, strlen(in2));
    ASSERT_STR_EQ((char *)s, "YSBsb25nZXIgYmFzZTY0IHRlc3QsIGFwcGFyZW50bHk=");
    free(s);

    char *out2 = "YSBsb25nZXIgYmFzZTY0IHRlc3QsIGFwcGFyZW50bHk=";
    size_t s_sz = 0;
    s = (unsigned char *) b64_decode(out2, strlen(out2), &s_sz);
    ASSERT_STR_EQ((char *)s, "a longer base64 test, apparently");
    ASSERT_EQ(strlen((char *)s), s_sz);
    free(s);

    s = hex_decode("DEADBEEF", &s_sz);
    unsigned char h[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
    for (size_t i = 0; i < 4; ++i) {
        ASSERT_EQ(s[i], h[i]);
    }
    free(s);

    s = hex_decode("f00f5", &s_sz);
    unsigned char h2[3] = { 0x0F, 0x00, 0xF5 };
    for (size_t i = 0; i < 3; ++i) {
        ASSERT_EQ(s[i], h2[i]);
    }
    free(s);

    s = hex_decode("0xf00f5", &s_sz);
    for (size_t i = 0; i < 3; ++i) {
        ASSERT_EQ(s[i], h2[i]);
    }
    free(s);

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
    unsigned int hamming = hamming_distance(ua, ub, 2);
    ASSERT_EQ(hamming, 3);

    hamming = hamming_distance_s("this is a test", "wokka wokka!!!");
    ASSERT_EQ(hamming, 37);

    TEST_REPORT();
}
