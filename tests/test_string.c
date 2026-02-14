#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfstring.h"

int main() {
    const char *haystack =
            "Test one two one and also maybe two but not Gabe's least favorite number, which is not one.";
    const char *needles[] = {
        "one",
        "two",
        "Gabe"
    };

    size_t sub_sz = 0;
    size_t *subs = NULL;
    find_substrings(haystack, needles[0], &sub_sz, &subs);

    ASSERT_EQ(sub_sz, 3);
    ASSERT_EQ(subs[0], 5);
    ASSERT_EQ(subs[1], 13);
    ASSERT_EQ(subs[2], 87);

    char *s = substr(haystack, subs[0], strlen(needles[0]));
    ASSERT_STR_EQ(s, needles[0]);

    free(s);
    free(subs);
    subs = NULL;

    find_substrings(haystack, needles[1], &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 2);
    ASSERT_EQ(subs[0], 9);

    free(subs);
    subs = NULL;

    find_substrings("test one two", "nope", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 0);
    ASSERT_NULL(subs);
    free(subs);
    subs = NULL;

    find_substrings("123", "nopes", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 0);
    ASSERT_NULL(subs);
    free(subs);
    subs = NULL;

    TEST_REPORT();
}
