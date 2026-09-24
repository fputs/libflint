#include "lfbytebuf.h"
#include "lftest.h"
int main(void) {
    unsigned char bytes[32] = {0};
    LfByteBuf b;
    ASSERT_EQ(lf_bytebuf_init(&b, NULL, 1), -1);
    ASSERT_EQ(lf_bytebuf_init(&b, NULL, 0), 0);
    ASSERT_EQ(lf_bytebuf_read(&b, NULL, 0), 0);
    ASSERT_EQ(lf_bytebuf_write(&b, NULL, 0), 0);
    ASSERT_EQ(lf_bytebuf_skip(&b, SIZE_MAX), -1);
    const unsigned char expected[] = {
        0xfe, 0x12, 0x34, 0x78, 0x56,
        0x12, 0x34, 0x56, 0x78, 0xef, 0xcd, 0xab, 0x90,
        1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1
    };
    /* Offset by one to exercise unaligned storage. */
    ASSERT_EQ(lf_bytebuf_init(&b, bytes + 1, sizeof(expected)), 0);
    ASSERT_EQ(lf_bytebuf_write_u8(&b, 0xfe), 0);
    ASSERT_EQ(lf_bytebuf_write_u16(&b, LF_BYTEBUF_BE, 0x1234), 0);
    ASSERT_EQ(lf_bytebuf_write_u16(&b, LF_BYTEBUF_LE, 0x5678), 0);
    ASSERT_EQ(lf_bytebuf_write_u32(&b, LF_BYTEBUF_BE, UINT32_C(0x12345678)), 0);
    ASSERT_EQ(lf_bytebuf_write_u32(&b, LF_BYTEBUF_LE, UINT32_C(0x90abcdef)), 0);
    ASSERT_EQ(lf_bytebuf_write_u64(&b, LF_BYTEBUF_BE, UINT64_C(0x0102030405060708)), 0);
    ASSERT_EQ(lf_bytebuf_write_u64(&b, LF_BYTEBUF_LE, UINT64_C(0x0102030405060708)), 0);
    ASSERT_EQ(memcmp(bytes + 1, expected, sizeof(expected)), 0);
    ASSERT_EQ(lf_bytebuf_remaining(&b), 0);
    ASSERT_EQ(lf_bytebuf_write_u8(&b, 1), -1); ASSERT_EQ(b.pos, sizeof(expected));
    ASSERT_EQ(lf_bytebuf_init_reader(&b, expected, sizeof(expected)), 0);
    ASSERT_EQ(lf_bytebuf_write_u8(&b, 1), -1); ASSERT_EQ(b.pos, 0);
    uint8_t u8; uint16_t u16; uint32_t u32; uint64_t u64;
    ASSERT_EQ(lf_bytebuf_read_u8(&b, &u8), 0); ASSERT_EQ(u8, 0xfe);
    ASSERT_EQ(lf_bytebuf_read_u16(&b, LF_BYTEBUF_BE, &u16), 0); ASSERT_EQ(u16, 0x1234);
    ASSERT_EQ(lf_bytebuf_read_u16(&b, LF_BYTEBUF_LE, &u16), 0); ASSERT_EQ(u16, 0x5678);
    ASSERT_EQ(lf_bytebuf_read_u32(&b, LF_BYTEBUF_BE, &u32), 0); ASSERT_EQ(u32, UINT32_C(0x12345678));
    ASSERT_EQ(lf_bytebuf_read_u32(&b, LF_BYTEBUF_LE, &u32), 0); ASSERT_EQ(u32, UINT32_C(0x90abcdef));
    ASSERT_EQ(lf_bytebuf_read_u64(&b, LF_BYTEBUF_BE, &u64), 0); ASSERT_EQ(u64, UINT64_C(0x0102030405060708));
    ASSERT_EQ(lf_bytebuf_read_u64(&b, LF_BYTEBUF_LE, &u64), 0); ASSERT_EQ(u64, UINT64_C(0x0102030405060708));
    ASSERT_EQ(lf_bytebuf_read_u64(&b, LF_BYTEBUF_LE, &u64), -1);
    ASSERT_EQ(u64, UINT64_C(0x0102030405060708));
    for (size_t n = 0; n < 8; n++) {
        memset(bytes, 0xab, sizeof(bytes));
        ASSERT_EQ(lf_bytebuf_init(&b, bytes, n), 0);
        ASSERT_EQ(lf_bytebuf_write_u64(&b, LF_BYTEBUF_LE, UINT64_MAX), -1);
        ASSERT_EQ(bytes[0], 0xab); ASSERT_EQ(b.pos, 0);
        ASSERT_EQ(lf_bytebuf_read_u64(&b, LF_BYTEBUF_BE, &u64), -1); ASSERT_EQ(b.pos, 0);
    }
    ASSERT_EQ(lf_bytebuf_init(&b, bytes, sizeof(bytes)), 0);
    ASSERT_EQ(lf_bytebuf_write_u32(&b, (LfByteOrder)99, 0), -1);
    ASSERT_EQ(lf_bytebuf_read_u32(&b, (LfByteOrder)99, &u32), -1);
    ASSERT_EQ(lf_bytebuf_read_u16(&b, LF_BYTEBUF_LE, NULL), -1); ASSERT_EQ(b.pos, 0);
    ASSERT_EQ(lf_bytebuf_write(&b, "abc", 3), 0);
    ASSERT_EQ(lf_bytebuf_write(&b, bytes, 3), 0);
    ASSERT_EQ(memcmp(bytes, "abcabc", 6), 0);
    ASSERT_EQ(lf_bytebuf_init_reader(&b, bytes, 6), 0);
    char out[3];
    ASSERT_EQ(lf_bytebuf_skip(&b, 3), 0);
    ASSERT_EQ(lf_bytebuf_read(&b, out, 3), 0); ASSERT_EQ(memcmp(out, "abc", 3), 0);
    ASSERT_EQ(lf_bytebuf_read(&b, out, 1), -1); ASSERT_EQ(memcmp(out, "abc", 3), 0);
    TEST_REPORT();
}
