#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfstring.h"

int main() {
    /* lfstr_find tests */
    const char *haystack =
            "Test one two one and also maybe two but not Gabe's least favorite number, which is not one.";
    const char *needles[] = {
        "one",
        "two",
        "Gabe"
    };

    size_t sub_sz = 0;
    size_t *subs = NULL;
    lfstr_find(haystack, needles[0], &sub_sz, &subs);

    ASSERT_EQ(sub_sz, 3);
    ASSERT_EQ(subs[0], 5);
    ASSERT_EQ(subs[1], 13);
    ASSERT_EQ(subs[2], 87);

    char *s = lfstr_substr(haystack, subs[0], strlen(needles[0]));
    ASSERT_STR_EQ(s, needles[0]);

    free(s);
    free(subs);
    subs = NULL;

    lfstr_find(haystack, needles[1], &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 2);
    ASSERT_EQ(subs[0], 9);

    free(subs);
    subs = NULL;

    lfstr_find("test one two", "nope", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 0);
    ASSERT_NULL(subs);
    free(subs);
    subs = NULL;

    lfstr_find("123", "nopes", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 0);
    ASSERT_NULL(subs);
    free(subs);
    subs = NULL;

    /* lfstr_find: empty needle matches every position, including one past the end */
    lfstr_find("abc", "", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 4);
    ASSERT_EQ(subs[0], 0);
    ASSERT_EQ(subs[1], 1);
    ASSERT_EQ(subs[2], 2);
    ASSERT_EQ(subs[3], 3);
    free(subs);
    subs = NULL;

    /* lfstr_find: empty haystack with empty needle */
    lfstr_find("", "", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 1);
    ASSERT_EQ(subs[0], 0);
    free(subs);
    subs = NULL;

    /* lfstr_find: empty haystack with non-empty needle */
    lfstr_find("", "abc", &sub_sz, &subs);
    ASSERT_EQ(sub_sz, 0);
    ASSERT_NULL(subs);
    subs = NULL;

    ASSERT_EQ(lfstr_find(NULL, "a", &sub_sz, &subs), -1);
    ASSERT_EQ(lfstr_find("a", NULL, &sub_sz, &subs), -1);
    ASSERT_EQ(lfstr_find("a", "a", NULL, &subs), -1);
    ASSERT_EQ(lfstr_find("a", "a", &sub_sz, NULL), -1);

    /* lfstr_substr edge cases */
    s = lfstr_substr("hello", 0, 5);
    ASSERT_STR_EQ(s, "hello");
    free(s);

    s = lfstr_substr("hello", 0, 0);
    ASSERT_STR_EQ(s, "");
    free(s);

    s = lfstr_substr("hello", 5, 0);
    ASSERT_STR_EQ(s, "");
    free(s);

    s = lfstr_substr("hello", 4, 1);
    ASSERT_STR_EQ(s, "o");
    free(s);

    /* substr beyond bounds */
    s = lfstr_substr("hello", 3, 5);
    ASSERT_NULL(s);

    s = lfstr_substr("hello", 0, 6);
    ASSERT_NULL(s);

    /* idx + len wrap must fail closed, not memcpy off the string */
    s = lfstr_substr("hello", SIZE_MAX - 1, 3);
    ASSERT_NULL(s);
    s = lfstr_substr("hello", SIZE_MAX, 1);
    ASSERT_NULL(s);
    s = lfstr_substr(NULL, 0, 0);
    ASSERT_NULL(s);

    /* lfstr_trim */
    s = lfstr_trim("  hello world  ");
    ASSERT_STR_EQ(s, "hello world");
    free(s);

    s = lfstr_trim("no_spaces");
    ASSERT_STR_EQ(s, "no_spaces");
    free(s);

    s = lfstr_trim("   ");
    ASSERT_STR_EQ(s, "");
    free(s);

    s = lfstr_trim("\t\n hi \r\n");
    ASSERT_STR_EQ(s, "hi");
    free(s);

    s = lfstr_trim("");
    ASSERT_STR_EQ(s, "");
    free(s);

    ASSERT_NULL(lfstr_trim(NULL));

    /* lfstr_join */
    const char *parts[] = {"foo", "bar", "baz"};
    s = lfstr_join(parts, 3, ", ");
    ASSERT_STR_EQ(s, "foo, bar, baz");
    free(s);

    s = lfstr_join(parts, 1, ", ");
    ASSERT_STR_EQ(s, "foo");
    free(s);

    s = lfstr_join(parts, 0, ", ");
    ASSERT_STR_EQ(s, "");
    free(s);

    s = lfstr_join(parts, 3, "");
    ASSERT_STR_EQ(s, "foobarbaz");
    free(s);

    ASSERT_NULL(lfstr_join(NULL, 3, ","));
    ASSERT_NULL(lfstr_join(parts, 3, NULL));

    /* lfstr_starts_with */
    ASSERT_TRUE(lfstr_starts_with("hello world", "hello"));
    ASSERT_TRUE(lfstr_starts_with("hello", "hello"));
    ASSERT_FALSE(lfstr_starts_with("hello", "world"));
    ASSERT_TRUE(lfstr_starts_with("hello", ""));
    ASSERT_TRUE(lfstr_starts_with("", ""));
    ASSERT_FALSE(lfstr_starts_with("", "a"));
    ASSERT_FALSE(lfstr_starts_with(NULL, "a"));
    ASSERT_FALSE(lfstr_starts_with("a", NULL));

    /* lfstr_ends_with */
    ASSERT_TRUE(lfstr_ends_with("hello world", "world"));
    ASSERT_TRUE(lfstr_ends_with("hello", "hello"));
    ASSERT_FALSE(lfstr_ends_with("hello", "world"));
    ASSERT_TRUE(lfstr_ends_with("hello", ""));
    ASSERT_FALSE(lfstr_ends_with("hi", "longer"));
    ASSERT_TRUE(lfstr_ends_with("", ""));
    ASSERT_FALSE(lfstr_ends_with("", "a"));

    /* lfstr_replace */
    s = lfstr_replace("foo bar foo baz foo", "foo", "qux");
    ASSERT_STR_EQ(s, "qux bar qux baz qux");
    free(s);

    s = lfstr_replace("aaa", "a", "bb");
    ASSERT_STR_EQ(s, "bbbbbb");
    free(s);

    s = lfstr_replace("hello", "xyz", "abc");
    ASSERT_STR_EQ(s, "hello");
    free(s);

    s = lfstr_replace("abcabc", "abc", "");
    ASSERT_STR_EQ(s, "");
    free(s);

    /* lfstr_replace with empty old string returns copy */
    s = lfstr_replace("hello", "", "X");
    ASSERT_STR_EQ(s, "hello");
    free(s);

    /* lfstr_replace on empty input */
    s = lfstr_replace("", "a", "b");
    ASSERT_STR_EQ(s, "");
    free(s);

    ASSERT_NULL(lfstr_replace(NULL, "a", "b"));
    ASSERT_NULL(lfstr_replace("a", NULL, "b"));
    ASSERT_NULL(lfstr_replace("a", "a", NULL));

    /* lfstr_to_upper / lfstr_to_lower */
    s = lfstr_to_upper("hello World 123");
    ASSERT_STR_EQ(s, "HELLO WORLD 123");
    free(s);

    s = lfstr_to_lower("HELLO World 123");
    ASSERT_STR_EQ(s, "hello world 123");
    free(s);

    s = lfstr_to_upper("");
    ASSERT_STR_EQ(s, "");
    free(s);

    s = lfstr_to_lower("");
    ASSERT_STR_EQ(s, "");
    free(s);

    ASSERT_NULL(lfstr_to_upper(NULL));
    ASSERT_NULL(lfstr_to_lower(NULL));

    /* lfstr_split / lfstr_split_free */
    size_t count = 0;
    char **split = lfstr_split("foo,bar,baz", ",", &count);
    ASSERT_EQ(count, 3);
    ASSERT_STR_EQ(split[0], "foo");
    ASSERT_STR_EQ(split[1], "bar");
    ASSERT_STR_EQ(split[2], "baz");
    lfstr_split_free(split, count);

    split = lfstr_split("hello", ",", &count);
    ASSERT_EQ(count, 1);
    ASSERT_STR_EQ(split[0], "hello");
    lfstr_split_free(split, count);

    split = lfstr_split(",a,,b,", ",", &count);
    ASSERT_EQ(count, 5);
    ASSERT_STR_EQ(split[0], "");
    ASSERT_STR_EQ(split[1], "a");
    ASSERT_STR_EQ(split[2], "");
    ASSERT_STR_EQ(split[3], "b");
    ASSERT_STR_EQ(split[4], "");
    lfstr_split_free(split, count);

    /* lfstr_split with multi-char delimiter */
    split = lfstr_split("a::b::c", "::", &count);
    ASSERT_EQ(count, 3);
    ASSERT_STR_EQ(split[0], "a");
    ASSERT_STR_EQ(split[1], "b");
    ASSERT_STR_EQ(split[2], "c");
    lfstr_split_free(split, count);

    /* lfstr_split with empty delimiter returns whole string */
    split = lfstr_split("hello", "", &count);
    ASSERT_EQ(count, 1);
    ASSERT_STR_EQ(split[0], "hello");
    lfstr_split_free(split, count);

    /* lfstr_split empty string */
    split = lfstr_split("", ",", &count);
    ASSERT_EQ(count, 1);
    ASSERT_STR_EQ(split[0], "");
    lfstr_split_free(split, count);

    ASSERT_NULL(lfstr_split(NULL, ",", &count));
    ASSERT_NULL(lfstr_split("a", NULL, &count));
    ASSERT_NULL(lfstr_split("a", ",", NULL));
    lfstr_split_free(NULL, 0);

    /* lfstr_contains */
    ASSERT_TRUE(lfstr_contains("hello world", "world"));
    ASSERT_TRUE(lfstr_contains("hello world", "hello"));
    ASSERT_TRUE(lfstr_contains("hello", "hello"));
    ASSERT_FALSE(lfstr_contains("hello", "xyz"));
    ASSERT_TRUE(lfstr_contains("hello", ""));
    ASSERT_TRUE(lfstr_contains("", ""));
    ASSERT_FALSE(lfstr_contains("", "a"));
    ASSERT_FALSE(lfstr_contains(NULL, "a"));
    ASSERT_FALSE(lfstr_contains("a", NULL));

    /* LfStr views: non-allocating predicates and trim */
    LfStr raw = lfstr_from("  hi  ");
    ASSERT_EQ(raw.n, 6);
    LfStr trimmed = lfstr_trim_view(raw);
    ASSERT_EQ(trimmed.n, 2);
    ASSERT_EQ(memcmp(trimmed.p, "hi", 2), 0);

    ASSERT_TRUE(lfstr_starts_with_view(lfstr_from("hello"), lfstr_from("he")));
    ASSERT_FALSE(lfstr_starts_with_view(lfstr_from("hello"), lfstr_from("lo")));
    ASSERT_TRUE(lfstr_ends_with_view(lfstr_from("hello"), lfstr_from("lo")));
    ASSERT_TRUE(lfstr_contains_view(lfstr_from("hello"), lfstr_from("ell")));
    ASSERT_TRUE(lfstr_contains_view(lfstr_from("hello"), (LfStr){NULL, 0}));

    LfStr empty = lfstr_from(NULL);
    ASSERT_NULL(empty.p);
    ASSERT_EQ(empty.n, 0);

    TEST_REPORT();
}
