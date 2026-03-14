#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfinput.h"

static void test_split_basic(void) {
    char *s = strdup("one,two,three");
    size_t n = 0;
    char **parts = inp_split(s, &n, ",");
    ASSERT_NOT_NULL(parts);
    ASSERT_EQ(n, 3);
    ASSERT_STR_EQ(parts[0], "one");
    ASSERT_STR_EQ(parts[1], "two");
    ASSERT_STR_EQ(parts[2], "three");
    inp_del_split(parts);
    free(s);
}

static void test_split_single(void) {
    char *s = strdup("hello");
    size_t n = 0;
    char **parts = inp_split(s, &n, ",");
    ASSERT_NOT_NULL(parts);
    ASSERT_EQ(n, 1);
    ASSERT_STR_EQ(parts[0], "hello");
    inp_del_split(parts);
    free(s);
}

static void test_split_null(void) {
    size_t n = 0;
    ASSERT_NULL(inp_split(NULL, &n, ","));
    char *s = strdup("a,b");
    ASSERT_NULL(inp_split(s, NULL, ","));
    free(s);
    s = strdup("a,b");
    ASSERT_NULL(inp_split(s, &n, NULL));
    free(s);
}

static void test_del_split_null(void) {
    inp_del_split(NULL);  /* should not crash */
}

static void test_del_lines_null(void) {
    inp_del_lines(NULL);  /* should not crash */
}

static void test_capture_system(void) {
    char *out = inp_capture_system("echo hello", 0);
    ASSERT_NOT_NULL(out);
    ASSERT_STR_EQ(out, "hello\n");
    free(out);
}

static void test_capture_system_null(void) {
    ASSERT_NULL(inp_capture_system(NULL, 0));
}

static void test_capture_system_large(void) {
    /* Generate output larger than the default buffer size */
    char *out = inp_capture_system("seq 1 1000", 32);
    ASSERT_NOT_NULL(out);
    /* Check it contains the last line */
    ASSERT_TRUE(strstr(out, "1000") != NULL);
    free(out);
}

static void test_file_roundtrip(void) {
    /* Write a temp file, read it back */
    const char *tmppath = "/tmp/libflint_test_input.txt";
    FILE *f = fopen(tmppath, "w");
    ASSERT_NOT_NULL(f);
    fprintf(f, "line1\nline2\nline3\n");
    fclose(f);

    char *contents = inp_get_input(tmppath);
    ASSERT_NOT_NULL(contents);
    ASSERT_STR_EQ(contents, "line1\nline2\nline3\n");
    free(contents);

    size_t n = 0;
    char **lines = inp_get_lines(tmppath, &n);
    ASSERT_NOT_NULL(lines);
    ASSERT_EQ(n, 3);
    ASSERT_STR_EQ(lines[0], "line1");
    ASSERT_STR_EQ(lines[1], "line2");
    ASSERT_STR_EQ(lines[2], "line3");
    inp_del_lines(lines);

    remove(tmppath);
}

static void test_get_binary(void) {
    const char *tmppath = "/tmp/libflint_test_input.bin";
    FILE *f = fopen(tmppath, "wb");
    ASSERT_NOT_NULL(f);
    unsigned char data[] = {0x00, 0x01, 0x02, 0xFF};
    fwrite(data, 1, sizeof(data), f);
    fclose(f);

    size_t fsz = 0;
    unsigned char *buf = inp_get_binary(tmppath, &fsz);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(fsz, 4);
    ASSERT_EQ(buf[0], 0x00);
    ASSERT_EQ(buf[3], 0xFF);
    free(buf);

    remove(tmppath);
}

static void test_get_ints(void) {
    const char *tmppath = "/tmp/libflint_test_ints.txt";
    FILE *f = fopen(tmppath, "w");
    ASSERT_NOT_NULL(f);
    fprintf(f, "10\n20\n30\n");
    fclose(f);

    size_t n = 0;
    int *ints = inp_get_ints(tmppath, &n);
    ASSERT_NOT_NULL(ints);
    ASSERT_EQ(n, 3);
    ASSERT_EQ(ints[0], 10);
    ASSERT_EQ(ints[1], 20);
    ASSERT_EQ(ints[2], 30);
    free(ints);

    remove(tmppath);
}

static void test_get_ints_null(void) {
    size_t n = 0;
    ASSERT_NULL(inp_get_ints(NULL, &n));
    ASSERT_NULL(inp_get_ints("/tmp/nonexistent_libflint", NULL));
}

int main(void) {
    test_split_basic();
    test_split_single();
    test_split_null();
    test_del_split_null();
    test_del_lines_null();
    test_capture_system();
    test_capture_system_null();
    test_capture_system_large();
    test_file_roundtrip();
    test_get_binary();
    test_get_ints();
    test_get_ints_null();

    TEST_REPORT();
}
