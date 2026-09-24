#include "lfstrbuf.h"
#include "lftest.h"
#include <stdlib.h>
int main(void) {
    LfStrBuf b;
    ASSERT_EQ(lf_strbuf_init(&b), 0); ASSERT_STR_EQ(b.data, "");
    ASSERT_EQ(lf_strbuf_append(&b, "hello"), 0);
    ASSERT_EQ(lf_strbuf_append_view(&b, (LfStr){b.data + 1, 3}), 0);
    ASSERT_STR_EQ(b.data, "helloell");
    ASSERT_EQ(lf_strbuf_append(&b, b.data), 0);
    ASSERT_STR_EQ(b.data, "helloellhelloell");
    ASSERT_EQ(lf_strbuf_appendf(&b, "[%s:%d]", b.data, 42), 0);
    ASSERT_STR_EQ(b.data, "helloellhelloell[helloellhelloell:42]");
    size_t len = b.len;
    char *old = b.data;
    ASSERT_EQ(lf_strbuf_reserve(&b, SIZE_MAX), -1); ASSERT_EQ(b.data, old);
    ASSERT_EQ(lf_strbuf_append_view(&b, (LfStr){"x", SIZE_MAX}), -1);
    ASSERT_EQ(lf_strbuf_append_view(&b, (LfStr){NULL, 1}), -1);
    ASSERT_EQ(b.len, len); ASSERT_EQ(b.data[len], 0);
    ASSERT_EQ(lf_strbuf_reserve(&b, 1024), 0); ASSERT_TRUE(b.cap >= 1025);
    lf_strbuf_clear(&b); ASSERT_EQ(b.len, 0); ASSERT_STR_EQ(b.data, "");
    ASSERT_EQ(lf_strbuf_append_view(&b, (LfStr){"a\0b", 3}), 0);
    ASSERT_EQ(b.len, 3); ASSERT_EQ(memcmp(b.data, "a\0b\0", 4), 0);
    char *owned = lf_strbuf_detach(&b); ASSERT_NOT_NULL(owned); ASSERT_NULL(b.data);
    ASSERT_EQ(memcmp(owned, "a\0b\0", 4), 0); free(owned);
    ASSERT_EQ(lf_strbuf_append(&b, "again"), 0); ASSERT_STR_EQ(b.data, "again");
    lf_strbuf_destroy(&b);
    owned = lf_strbuf_detach(&b); ASSERT_STR_EQ(owned, ""); free(owned);
    ASSERT_EQ(lf_strbuf_append_view(&b, (LfStr){NULL, 0}), 0); ASSERT_STR_EQ(b.data, "");
    lf_strbuf_destroy(&b); lf_strbuf_destroy(&b);
    TEST_REPORT();
}
