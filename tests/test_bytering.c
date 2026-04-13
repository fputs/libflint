#include <string.h>
#include "lftest.h"
#include "lfbytering.h"

static void test_init() {
    uint8_t buf[8];
    ByteRing br;

    ASSERT_EQ(br_init(&br, buf, 8), 0);
    ASSERT_EQ(br_capacity(&br), 8);
    ASSERT_EQ(br_available(&br), 0);
    ASSERT_EQ(br_free_space(&br), 7);

    /* non-power-of-2 rejected */
    ASSERT_EQ(br_init(&br, buf, 3), -1);
    ASSERT_EQ(br_init(&br, buf, 10), -1);

    /* zero and null rejected */
    ASSERT_EQ(br_init(&br, buf, 0), -1);
    ASSERT_EQ(br_init(&br, NULL, 8), -1);
    ASSERT_EQ(br_init(NULL, buf, 8), -1);
}

static void test_read_write() {
    uint8_t buf[8];
    ByteRing br;
    br_init(&br, buf, 8);

    /* write and read single bytes */
    ASSERT_EQ(br_write(&br, 0xAA), 0);
    ASSERT_EQ(br_write(&br, 0xBB), 0);
    ASSERT_EQ(br_available(&br), 2);

    uint8_t byte;
    ASSERT_EQ(br_read(&br, &byte), 0);
    ASSERT_EQ(byte, 0xAA);
    ASSERT_EQ(br_read(&br, &byte), 0);
    ASSERT_EQ(byte, 0xBB);

    /* read when empty */
    ASSERT_EQ(br_read(&br, &byte), -1);
    ASSERT_EQ(br_available(&br), 0);
}

static void test_peek() {
    uint8_t buf[4];
    ByteRing br;
    br_init(&br, buf, 4);

    /* peek when empty */
    uint8_t byte;
    ASSERT_EQ(br_peek(&br, &byte), -1);

    br_write(&br, 0x42);
    ASSERT_EQ(br_peek(&br, &byte), 0);
    ASSERT_EQ(byte, 0x42);

    /* peek doesn't consume */
    ASSERT_EQ(br_available(&br), 1);
}

static void test_full() {
    uint8_t buf[4];
    ByteRing br;
    br_init(&br, buf, 4);

    /* capacity 4, usable slots = 3 */
    ASSERT_EQ(br_free_space(&br), 3);
    ASSERT_EQ(br_write(&br, 0x01), 0);
    ASSERT_EQ(br_write(&br, 0x02), 0);
    ASSERT_EQ(br_write(&br, 0x03), 0);
    ASSERT_EQ(br_free_space(&br), 0);

    /* write when full */
    ASSERT_EQ(br_write(&br, 0x04), -1);
    ASSERT_EQ(br_available(&br), 3);
}

static void test_wrap() {
    uint8_t buf[4];
    ByteRing br;
    br_init(&br, buf, 4);

    /* fill to capacity */
    br_write(&br, 0x01);
    br_write(&br, 0x02);
    br_write(&br, 0x03);

    /* pop two, push two more to force tail wrap */
    uint8_t byte;
    br_read(&br, &byte);
    ASSERT_EQ(byte, 0x01);
    br_read(&br, &byte);
    ASSERT_EQ(byte, 0x02);

    br_write(&br, 0x04);
    br_write(&br, 0x05);

    /* read back in FIFO order across the wrap */
    br_read(&br, &byte);
    ASSERT_EQ(byte, 0x03);
    br_read(&br, &byte);
    ASSERT_EQ(byte, 0x04);
    br_read(&br, &byte);
    ASSERT_EQ(byte, 0x05);
    ASSERT_EQ(br_available(&br), 0);
}

static void test_bulk_write_read() {
    uint8_t buf[8];
    ByteRing br;
    br_init(&br, buf, 8);

    uint8_t src[] = {0x10, 0x20, 0x30, 0x40, 0x50};
    ASSERT_EQ(br_bulk_write(&br, 5, src), 5);
    ASSERT_EQ(br_available(&br), 5);

    uint8_t dest[5];
    ASSERT_EQ(br_bulk_read(&br, 5, dest), 5);
    ASSERT_EQ(dest[0], 0x10);
    ASSERT_EQ(dest[1], 0x20);
    ASSERT_EQ(dest[2], 0x30);
    ASSERT_EQ(dest[3], 0x40);
    ASSERT_EQ(dest[4], 0x50);
    ASSERT_EQ(br_available(&br), 0);
}

static void test_bulk_wrap() {
    uint8_t buf[8];
    ByteRing br;
    br_init(&br, buf, 8);

    /* advance head and tail to index 5 */
    uint8_t src[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
    br_bulk_write(&br, 5, src);
    uint8_t discard[5];
    br_bulk_read(&br, 5, discard);

    /* head=5, tail=5. Write 5 bytes: wraps tail around end of buffer */
    uint8_t src2[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    ASSERT_EQ(br_bulk_write(&br, 5, src2), 5);

    /* read back across the wrap */
    uint8_t dest[5];
    ASSERT_EQ(br_bulk_read(&br, 5, dest), 5);
    ASSERT_EQ(dest[0], 0x01);
    ASSERT_EQ(dest[1], 0x02);
    ASSERT_EQ(dest[2], 0x03);
    ASSERT_EQ(dest[3], 0x04);
    ASSERT_EQ(dest[4], 0x05);
}

static void test_bulk_partial() {
    uint8_t buf[4];
    ByteRing br;
    br_init(&br, buf, 4);

    /* only 3 usable slots, try to write 5 */
    uint8_t src[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    ASSERT_EQ(br_bulk_write(&br, 5, src), 3);
    ASSERT_EQ(br_available(&br), 3);

    /* try to read 5, only 3 available */
    uint8_t dest[5];
    memset(dest, 0, 5);
    ASSERT_EQ(br_bulk_read(&br, 5, dest), 3);
    ASSERT_EQ(dest[0], 0x01);
    ASSERT_EQ(dest[1], 0x02);
    ASSERT_EQ(dest[2], 0x03);
}

static void test_reset() {
    uint8_t buf[8];
    ByteRing br;
    br_init(&br, buf, 8);

    br_write(&br, 0x01);
    br_write(&br, 0x02);
    ASSERT_EQ(br_available(&br), 2);

    br_reset(&br);
    ASSERT_EQ(br_available(&br), 0);

    /* usable after reset */
    ASSERT_EQ(br_write(&br, 0xFF), 0);
    uint8_t byte;
    ASSERT_EQ(br_read(&br, &byte), 0);
    ASSERT_EQ(byte, 0xFF);
}

static void test_bulk_zero() {
    uint8_t buf[8];
    ByteRing br;
    br_init(&br, buf, 8);

    br_write(&br, 0x01);

    /* zero-length bulk write does nothing */
    uint8_t src[] = {0xAA};
    ASSERT_EQ(br_bulk_write(&br, 0, src), 0);
    ASSERT_EQ(br_available(&br), 1);

    /* zero-length bulk read does nothing */
    uint8_t dest[1];
    ASSERT_EQ(br_bulk_read(&br, 0, dest), 0);
    ASSERT_EQ(br_available(&br), 1);
}

int main() {
    test_init();
    test_read_write();
    test_peek();
    test_full();
    test_wrap();
    test_bulk_write_read();
    test_bulk_wrap();
    test_bulk_partial();
    test_reset();
    test_bulk_zero();
    TEST_REPORT();
}
