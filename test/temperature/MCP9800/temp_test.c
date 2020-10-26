#include "temperature/MCP9800/MCP9800.h"
#include "unity.h"

void setUp() {}
void tearDown() {}

static int test1 = 0;

int write_i2c_reg(uint8_t devaddr, uint8_t *write, int writelen, uint8_t *data, int len) {
    (void)data;
    test1 = 1;
    TEST_ASSERT_EQUAL(0x98, devaddr);
    TEST_ASSERT_EQUAL(MCP9800_TEMPERATURE_REGISTER, *write);
    TEST_ASSERT_EQUAL(1, writelen);
    TEST_ASSERT_EQUAL(2, len);

    data[0] = 0x8A;
    data[1] = 0x80;

    return 0;
}

void test_temp() {
    i2c_driver_t driver = {.device_address = 0x98, .i2c_transfer = write_i2c_reg};

    TEST_ASSERT(-10.5 == MCP9800_read_temperature(driver));
    TEST_ASSERT_EQUAL(1, test1);
}