#include "i2c_common.h"

int i2c_read_register(i2c_driver_t driver, uint8_t reg, uint8_t *data, size_t len) {
    uint8_t buf = reg;
    return driver.i2c_transfer(driver.device_address, &buf, 1, data, len);
}

int i2c_write_register(i2c_driver_t driver, uint8_t reg, uint8_t *data, size_t len) {
    uint8_t buffer[len + 1];
    buffer[0] = reg;
    memcpy(&buffer[1], data, len);
    return driver.i2c_transfer(driver.device_address, buffer, len + 1, NULL, 0);
}
