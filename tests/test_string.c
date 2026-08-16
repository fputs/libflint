#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfstring.h"

int main() {
    /* str_find tests */
    const char *haystack =
            "Test one two one and also maybe two but not Gabe's least favorite number, which is not one.";
    const char *needles[] = {
        "one",
        "two",
        "Gabe"
    };

    size_t sub_sz = 0;
    size_t *subs = NULL;
    str_find(haystack, needles[0], &sub_sz, &subs);

    ASSERT_EQ(sub_sz, 3);
    ASSERT_EQ(subs[0], 5);
    ASSERT_EQ(subs[1], 13);
    ASSERT_EQ(subs[2], 87);

    char *s = str_substr(haystack, subs[0], strlen(needles[0]));
    ASSERT_STR_EQ(s, needles[0]);

    free(s);
    free(subs);
    subs = NULL;

    str_find(haystack, needles[1], &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 2);
    ASSERT_EQ(subs[0], 9);

    free(subs);
    subs = NULL;

    str_find("test one two", "nope", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 0);
    ASSERT_NULL(subs);
    free(subs);
    subs = NULL;

    str_find("123", "nopes", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 0);
    ASSERT_NULL(subs);
    free(subs);
    subs = NULL;

    /* str_find: empty needle matches every position */
    str_find("abc", "", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 4);
    free(subs);
    subs = NULL;

    /* str_find: empty haystack with empty needle */
    str_find("", "", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 1);
    free(subs);
    subs = NULL;

    /* str_find: empty haystack with non-empty needle */
    str_find("", "abc", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 0);
    ASSERT_NULL(subs);
    subs = NULL;

    /* str_substr edge cases */
    s = str_substr("hello", 0, 5);
    ASSERT_STR_EQ(s, "hello");
    free(s);

    s = str_substr("hello", 0, 0);
    ASSERT_STR_EQ(s, "");
    free(s);

    s = str_substr("hello", 5, 0);
    ASSERT_STR_EQ(s, "");
    free(s);

    s = str_substr("hello", 4, 1);
    ASSERT_STR_EQ(s, "o");
    free(s);

    /* substr beyond bounds */
    s = str_substr("hello", 3, 5);
    ASSERT_NULL(s);

    s = str_substr("hello", 0, 6);
    ASSERT_NULL(s);

    /* idx + len wrap must fail closed, not memcpy off the string */
    s = str_substr("hello", SIZE_MAX - 1, 3);
    ASSERT_NULL(s);
    s = str_substr("hello", SIZE_MAX, 1);
    ASSERT_NULL(s);
    s = str_substr(NULL, 0, 0);
    ASSERT_NULL(s);

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

    s = str_trim("");
    ASSERT_STR_EQ(s, "");
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

    ASSERT_NULL(str_join(NULL, 3, ","));
    ASSERT_NULL(str_join(parts, 3, NULL));

    /* str_starts_with */
    ASSERT_TRUE(str_starts_with("hello world", "hello"));
    ASSERT_TRUE(str_starts_with("hello", "hello"));
    ASSERT_FALSE(str_starts_with("hello", "world"));
    ASSERT_TRUE(str_starts_with("hello", ""));
    ASSERT_TRUE(str_starts_with("", ""));
    ASSERT_FALSE(str_starts_with("", "a"));

    /* str_ends_with */
    ASSERT_TRUE(str_ends_with("hello world", "world"));
    ASSERT_TRUE(str_ends_with("hello", "hello"));
    ASSERT_FALSE(str_ends_with("hello", "world"));
    ASSERT_TRUE(str_ends_with("hello", ""));
    ASSERT_FALSE(str_ends_with("hi", "longer"));
    ASSERT_TRUE(str_ends_with("", ""));
    ASSERT_FALSE(str_ends_with("", "a"));

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

    /* str_replace with empty old string returns copy */
    s = str_replace("hello", "", "X");
    ASSERT_STR_EQ(s, "hello");
    free(s);

    /* str_replace on empty input */
    s = str_replace("", "a", "b");
    ASSERT_STR_EQ(s, "");
    free(s);

    ASSERT_NULL(str_replace(NULL, "a", "b"));
    ASSERT_NULL(str_replace("a", NULL, "b"));
    ASSERT_NULL(str_replace("a", "a", NULL));

    /* str_to_upper / str_to_lower */
    s = str_to_upper("hello World 123");
    ASSERT_STR_EQ(s, "HELLO WORLD 123");
    free(s);

    s = str_to_lower("HELLO World 123");
    ASSERT_STR_EQ(s, "hello world 123");
    free(s);

    s = str_to_upper("");
    ASSERT_STR_EQ(s, "");
    free(s);

    s = str_to_lower("");
    ASSERT_STR_EQ(s, "");
    free(s);

    /* str_split */
    size_t count = 0;
    char **split = str_split("foo,bar,baz", ",", &count);
    ASSERT_EQ(count, 3);
    ASSERT_STR_EQ(split[0], "foo");
    ASSERT_STR_EQ(split[1], "bar");
    ASSERT_STR_EQ(split[2], "baz");
    for (size_t i = 0; i < count; i++) free(split[i]);
    free(split);

    split = str_split("hello", ",", &count);
    ASSERT_EQ(count, 1);
    ASSERT_STR_EQ(split[0], "hello");
    free(split[0]);
    free(split);

    split = str_split(",a,,b,", ",", &count);
    ASSERT_EQ(count, 5);
    ASSERT_STR_EQ(split[0], "");
    ASSERT_STR_EQ(split[1], "a");
    ASSERT_STR_EQ(split[2], "");
    ASSERT_STR_EQ(split[3], "b");
    ASSERT_STR_EQ(split[4], "");
    for (size_t i = 0; i < count; i++) free(split[i]);
    free(split);

    /* str_split with multi-char delimiter */
    split = str_split("a::b::c", "::", &count);
    ASSERT_EQ(count, 3);
    ASSERT_STR_EQ(split[0], "a");
    ASSERT_STR_EQ(split[1], "b");
    ASSERT_STR_EQ(split[2], "c");
    for (size_t i = 0; i < count; i++) free(split[i]);
    free(split);

    /* str_split with empty delimiter returns whole string */
    split = str_split("hello", "", &count);
    ASSERT_EQ(count, 1);
    ASSERT_STR_EQ(split[0], "hello");
    free(split[0]);
    free(split);

    /* str_split empty string */
    split = str_split("", ",", &count);
    ASSERT_EQ(count, 1);
    ASSERT_STR_EQ(split[0], "");
    free(split[0]);
    free(split);

    /* str_contains */
    ASSERT_TRUE(str_contains("hello world", "world"));
    ASSERT_TRUE(str_contains("hello world", "hello"));
    ASSERT_TRUE(str_contains("hello", "hello"));
    ASSERT_FALSE(str_contains("hello", "xyz"));
    ASSERT_TRUE(str_contains("hello", ""));
    ASSERT_TRUE(str_contains("", ""));
    ASSERT_FALSE(str_contains("", "a"));

    TEST_REPORT();
}
