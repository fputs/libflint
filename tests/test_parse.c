#include "lfparse.h"
#include "lftest.h"

int main(void) {
    LfStr s = lfstr_from("18446744073709551615!");
    uint64_t u = 0;
    int64_t i = 0;
    ASSERT_EQ(lf_parse_u64(&s, 10, &u), LF_PARSE_OK); ASSERT_EQ(u, UINT64_MAX);
    ASSERT_EQ(s.n, 1); ASSERT_EQ(*s.p, '!');
    s = lfstr_from("-9223372036854775808,");
    ASSERT_EQ(lf_parse_i64(&s, 10, &i), LF_PARSE_OK); ASSERT_EQ(i, INT64_MIN); ASSERT_EQ(s.n, 1);
    s = lfstr_from("+9223372036854775807");
    ASSERT_EQ(lf_parse_i64(&s, 10, &i), LF_PARSE_OK); ASSERT_EQ(i, INT64_MAX); ASSERT_EQ(s.n, 0);
    const char *overflow[] = {"9223372036854775808", "-9223372036854775809", "99999999999999999999999999"};
    for (size_t k = 0; k < 3; k++) {
        s = lfstr_from(overflow[k]); LfStr old = s; i = 42;
        ASSERT_EQ(lf_parse_i64(&s, 10, &i), LF_PARSE_OVERFLOW);
        ASSERT_EQ(s.p, old.p); ASSERT_EQ(s.n, old.n); ASSERT_EQ(i, 42);
    }
    s = lfstr_from("18446744073709551616"); LfStr old = s; u = 42;
    ASSERT_EQ(lf_parse_u64(&s, 10, &u), LF_PARSE_OVERFLOW);
    ASSERT_EQ(s.p, old.p); ASSERT_EQ(s.n, old.n); ASSERT_EQ(u, 42);
    const char *invalid[] = {"", "+", "-", "--1", " 1", "!", "\xff"};
    for (size_t k = 0; k < sizeof(invalid) / sizeof(*invalid); k++) {
        s = lfstr_from(invalid[k]); old = s;
        ASSERT_EQ(lf_parse_i64(&s, 10, &i), LF_PARSE_INVALID);
        ASSERT_EQ(s.p, old.p); ASSERT_EQ(s.n, old.n);
    }
    s = lfstr_from("-0"); ASSERT_EQ(lf_parse_u64(&s, 10, &u), LF_PARSE_INVALID);
    ASSERT_EQ(lf_parse_i64(&s, 10, &i), LF_PARSE_OK); ASSERT_EQ(i, 0);
    for (unsigned base = 2; base <= 36; base++) {
        s = lfstr_from("+10!"); ASSERT_EQ(lf_parse_u64(&s, base, &u), LF_PARSE_OK);
        ASSERT_EQ(u, base); ASSERT_EQ(s.n, 1);
    }
    s = lfstr_from("fFz"); ASSERT_EQ(lf_parse_u64(&s, 16, &u), LF_PARSE_OK); ASSERT_EQ(u, 255); ASSERT_EQ(s.n, 1);
    s = lfstr_from("Zz"); ASSERT_EQ(lf_parse_u64(&s, 36, &u), LF_PARSE_OK); ASSERT_EQ(u, 1295);
    s = lfstr_from("0xff"); ASSERT_EQ(lf_parse_u64(&s, 16, &u), LF_PARSE_OK); ASSERT_EQ(u, 0); ASSERT_EQ(s.n, 3);
    s = lfstr_from("12"); ASSERT_EQ(lf_parse_u64(&s, 0, &u), LF_PARSE_INVALID);
    ASSERT_EQ(lf_parse_i64(&s, 37, &i), LF_PARSE_INVALID); ASSERT_EQ(s.n, 2);
    char raw[] = {'4', '2', '\0', 'x'};
    s = (LfStr){raw, sizeof(raw)}; ASSERT_EQ(lf_parse_u64(&s, 10, &u), LF_PARSE_OK);
    ASSERT_EQ(u, 42); ASSERT_EQ(s.n, 2);
    s = (LfStr){NULL, 0}; ASSERT_EQ(lf_parse_u64(&s, 10, &u), LF_PARSE_INVALID);
    ASSERT_EQ(lf_parse_consume(&s, (LfStr){NULL, 0}), LF_PARSE_OK);
    ASSERT_EQ(lf_parse_skip_whitespace(&s), 0);
    s = lfstr_from(" \t\r\n\f\vname=42,tail");
    ASSERT_EQ(lf_parse_skip_whitespace(&s), 6);
    LfStr token = {NULL, 0};
    ASSERT_EQ(lf_parse_token(&s, lfstr_from("=,"), &token), LF_PARSE_OK);
    ASSERT_EQ(token.n, 4); ASSERT_EQ(memcmp(token.p, "name", 4), 0);
    old = s;
    ASSERT_EQ(lf_parse_consume(&s, lfstr_from("==")), LF_PARSE_INVALID);
    ASSERT_EQ(s.p, old.p); ASSERT_EQ(s.n, old.n);
    ASSERT_EQ(lf_parse_token(&s, lfstr_from("="), &token), LF_PARSE_INVALID); ASSERT_EQ(token.n, 4);
    ASSERT_EQ(lf_parse_consume(&s, lfstr_from("=")), LF_PARSE_OK);
    ASSERT_EQ(lf_parse_i64(&s, 10, &i), LF_PARSE_OK); ASSERT_EQ(i, 42);
    ASSERT_EQ(lf_parse_consume(&s, lfstr_from(",")), LF_PARSE_OK);
    ASSERT_EQ(lf_parse_token(&s, (LfStr){NULL, 0}, &token), LF_PARSE_OK); ASSERT_EQ(token.n, 4); ASSERT_EQ(s.n, 0);
    s = (LfStr){raw, sizeof(raw)};
    ASSERT_EQ(lf_parse_token(&s, (LfStr){"\0", 1}, &token), LF_PARSE_OK); ASSERT_EQ(token.n, 2);
    ASSERT_EQ(lf_parse_token(&s, lfstr_from("x"), &s), LF_PARSE_INVALID);
    TEST_REPORT();
}
