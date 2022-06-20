#include "i2c_common.h"


int i2c_read_register(i2c_driver_t driver, uint8_t reg, uint8_t *data, size_t len) {
    uint8_t buf = reg;
    return driver.i2c_transfer(driver.device_address, &buf, 1, data, len, driver.arg);
}


int i2c_write_register(i2c_driver_t driver, uint8_t reg, uint8_t *data, size_t len) {
    uint8_t buffer[len + 1];
    buffer[0] = reg;
    memcpy(&buffer[1], data, len);
    return driver.i2c_transfer(driver.device_address, buffer, len + 1, NULL, 0, driver.arg);
}


uint8_t i2c_common_crc8(uint8_t polynomial, uint8_t crc, uint8_t *data, size_t len) {
    size_t i, j;
    for (j = len; j; j--) {
        crc ^= *data++;
        for (i = 8; i; i--) {
            crc = (crc & 0x80) ? (crc << 1) ^ polynomial : (crc << 1);
        }
    }
    return crc;
}