#include "EEPROM/24LC16.h"
#include "unity.h"

void setUp() {}
void tearDown() {}

static int test1 = 0;
static int test2 = 0;

void i2c_ack_polling(uint8_t devaddr) {
    TEST_ASSERT_EQUAL(0xA0, devaddr);
    test2 = 1;
}

int write_i2c_reg(uint8_t devaddr, uint8_t *write, int writelen, uint8_t *data, int len) {
    (void)data;
    (void)write;
    (void)writelen;
    (void)len;

    test1++;
    TEST_ASSERT(*write % 0x10 == 0 || *write == 5);
    TEST_ASSERT_EQUAL(0xA0, devaddr & 0xF1);
    TEST_ASSERT(len < 0x10);

    if (test1 > 0x30)
        TEST_ASSERT_EQUAL(3, (devaddr & 0x7) >> 1);
    else if (test1 > 0x20)
        TEST_ASSERT_EQUAL(2, (devaddr & 0x7) >> 1);
    else if (test1 > 0x10)
        TEST_ASSERT_EQUAL(1, (devaddr & 0x7) >> 1);
    else
        TEST_ASSERT_EQUAL(0, (devaddr & 0x7) >> 1);


    return 0;
}

void test_temp() {
    uint8_t      buffer[BLOCK_SIZE * 3];
    i2c_driver_t driver = {0xA0, write_i2c_reg, i2c_ack_polling};

    EE24LC16_sequential_write(driver, 0, 5, buffer, BLOCK_SIZE * 3);
    TEST_ASSERT_EQUAL(1, test2);

    TEST_ASSERT_EQUAL(-2, EE24LC16_sequential_write(driver, 0, 0, buffer, MEM_SIZE + 1));
    TEST_ASSERT_EQUAL(-1, EE24LC16_sequential_write(driver, 10, 0, buffer, 1));
}