#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfinput.h"
#include "lfprocess.h"

static void test_split_basic(void) {
    char *s = strdup("one,two,three");
    LfLines parts;
    ASSERT_EQ(inp_split(s, ",", &parts), 0);
    ASSERT_EQ(parts.n, 3);
    ASSERT_NULL(parts.base);
    ASSERT_STR_EQ(parts.lines[0], "one");
    ASSERT_STR_EQ(parts.lines[1], "two");
    ASSERT_STR_EQ(parts.lines[2], "three");
    inp_lines_free(&parts);
    free(s);
}

static void test_split_single(void) {
    char *s = strdup("hello");
    LfLines parts;
    ASSERT_EQ(inp_split(s, ",", &parts), 0);
    ASSERT_EQ(parts.n, 1);
    ASSERT_STR_EQ(parts.lines[0], "hello");
    inp_lines_free(&parts);
    free(s);
}

static void test_split_null(void) {
    LfLines parts;
    ASSERT_EQ(inp_split(NULL, ",", &parts), -1);
    char *s = strdup("a,b");
    ASSERT_EQ(inp_split(s, ",", NULL), -1);
    free(s);
    s = strdup("a,b");
    ASSERT_EQ(inp_split(s, NULL, &parts), -1);
    free(s);
}

static void test_lines_free_null(void) {
    inp_lines_free(NULL);
}

static void test_capture_system(void) {
    char *out = lf_capture_system("echo hello", 0);
    ASSERT_NOT_NULL(out);
    ASSERT_STR_EQ(out, "hello\n");
    free(out);
}

static void test_capture_system_null(void) {
    ASSERT_NULL(lf_capture_system(NULL, 0));
}

static void test_capture_system_large(void) {
    char *out = lf_capture_system("seq 1 1000", 32);
    ASSERT_NOT_NULL(out);
    ASSERT_TRUE(strstr(out, "1000") != NULL);
    free(out);
}

static void test_file_roundtrip(void) {
    const char *tmppath = "/tmp/libflint_test_input.txt";
    FILE *f = fopen(tmppath, "w");
    ASSERT_NOT_NULL(f);
    fprintf(f, "line1\nline2\nline3\n");
    fclose(f);

    char *contents = inp_get_input(tmppath);
    ASSERT_NOT_NULL(contents);
    ASSERT_STR_EQ(contents, "line1\nline2\nline3\n");
    free(contents);

    LfLines lines;
    ASSERT_EQ(inp_get_lines(tmppath, &lines), 0);
    ASSERT_NOT_NULL(lines.base);
    ASSERT_EQ(lines.n, 3);
    ASSERT_STR_EQ(lines.lines[0], "line1");
    ASSERT_STR_EQ(lines.lines[1], "line2");
    ASSERT_STR_EQ(lines.lines[2], "line3");
    inp_lines_free(&lines);

    remove(tmppath);
}

static void test_crlf(void) {
    const char *tmppath = "/tmp/libflint_test_crlf.txt";
    FILE *f = fopen(tmppath, "wb");
    ASSERT_NOT_NULL(f);
    fwrite("a\r\nb\r\n", 1, 6, f);
    fclose(f);

    LfLines lines;
    ASSERT_EQ(inp_get_lines(tmppath, &lines), 0);
    ASSERT_EQ(lines.n, 2);
    ASSERT_STR_EQ(lines.lines[0], "a");
    ASSERT_STR_EQ(lines.lines[1], "b");
    inp_lines_free(&lines);
    remove(tmppath);
}

static void test_empty_vs_missing(void) {
    const char *tmppath = "/tmp/libflint_test_empty.txt";
    FILE *f = fopen(tmppath, "w");
    ASSERT_NOT_NULL(f);
    fclose(f);

    LfLines lines;
    memset(&lines, 0x5a, sizeof lines);
    ASSERT_EQ(inp_get_lines(tmppath, &lines), 0);
    ASSERT_EQ(lines.n, 0);
    ASSERT_NULL(lines.lines);
    ASSERT_NOT_NULL(lines.base);
    inp_lines_free(&lines);

    memset(&lines, 0x5a, sizeof lines);
    ASSERT_EQ(inp_get_lines("/tmp/libflint_no_such_file_xyz", &lines), -1);
    unsigned char *raw = (unsigned char *)&lines;
    ASSERT_EQ(raw[0], (unsigned char)0x5a);

    ASSERT_NULL(inp_get_input("/tmp/libflint_no_such_file_xyz"));

    size_t fsz = 99;
    unsigned char *empty = inp_get_binary(tmppath, &fsz);
    ASSERT_NOT_NULL(empty);
    ASSERT_EQ(fsz, (size_t)0);
    free(empty);

    ASSERT_NULL(inp_get_binary(tmppath, NULL));
    ASSERT_NULL(inp_get_binary("/tmp/libflint_no_such_file_xyz", &fsz));

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

    /* empty file is a freeable zero-length list */
    f = fopen(tmppath, "w");
    fclose(f);
    ints = inp_get_ints(tmppath, &n);
    ASSERT_NOT_NULL(ints);
    ASSERT_EQ(n, (size_t)0);
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
    test_lines_free_null();
    test_capture_system();
    test_capture_system_null();
    test_capture_system_large();
    test_file_roundtrip();
    test_crlf();
    test_empty_vs_missing();
    test_get_binary();
    test_get_ints();
    test_get_ints_null();

    TEST_REPORT();
}
