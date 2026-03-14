#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfstring.h"

int main() {
    /* existing find_substrings tests */
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

    /* str_trim */
    s = str_trim("  hello world  ");
    ASSERT_STR_EQ(s, "hello world");
    free(s);

    s = str_trim("no_spaces");
    ASSERT_STR_EQ(s, "no_spaces");
    free(s);

    s = str_trim("   ");
    ASSERT_STR_EQ(s, "");
    free(s);

    s = str_trim("\t\n hi \r\n");
    ASSERT_STR_EQ(s, "hi");
    free(s);

    /* str_join */
    const char *parts[] = {"foo", "bar", "baz"};
    s = str_join(parts, 3, ", ");
    ASSERT_STR_EQ(s, "foo, bar, baz");
    free(s);

    s = str_join(parts, 1, ", ");
    ASSERT_STR_EQ(s, "foo");
    free(s);

    s = str_join(parts, 0, ", ");
    ASSERT_STR_EQ(s, "");
    free(s);

    s = str_join(parts, 3, "");
    ASSERT_STR_EQ(s, "foobarbaz");
    free(s);

    /* str_starts_with */
    ASSERT_TRUE(str_starts_with("hello world", "hello"));
    ASSERT_TRUE(str_starts_with("hello", "hello"));
    ASSERT_FALSE(str_starts_with("hello", "world"));
    ASSERT_TRUE(str_starts_with("hello", ""));

    /* str_ends_with */
    ASSERT_TRUE(str_ends_with("hello world", "world"));
    ASSERT_TRUE(str_ends_with("hello", "hello"));
    ASSERT_FALSE(str_ends_with("hello", "world"));
    ASSERT_TRUE(str_ends_with("hello", ""));
    ASSERT_FALSE(str_ends_with("hi", "longer"));

    /* str_replace */
    s = str_replace("foo bar foo baz foo", "foo", "qux");
    ASSERT_STR_EQ(s, "qux bar qux baz qux");
    free(s);

    s = str_replace("aaa", "a", "bb");
    ASSERT_STR_EQ(s, "bbbbbb");
    free(s);

    s = str_replace("hello", "xyz", "abc");
    ASSERT_STR_EQ(s, "hello");
    free(s);

    s = str_replace("abcabc", "abc", "");
    ASSERT_STR_EQ(s, "");
    free(s);

    /* str_to_upper / str_to_lower */
    s = str_to_upper("hello World 123");
    ASSERT_STR_EQ(s, "HELLO WORLD 123");
    free(s);

    s = str_to_lower("HELLO World 123");
    ASSERT_STR_EQ(s, "hello world 123");
    free(s);

    TEST_REPORT();
}
