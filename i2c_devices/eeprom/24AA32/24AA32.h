#ifndef EE24AA16_H_INCLUDED
#define EE24AA16_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"


#define EEPROM24AA32_DEFAULT_ADDRESS 0xA0


#define EE24AA32_ABSOLUTE_WRITE(driver, address, data, len)                                                            \
    ee24aa32_sequential_write(driver, (address >> 8) & 0xFF, address & 0xFF, data, len)
#define EE24AA32_ABSOLUTE_READ(driver, address, data, len)                                                             \
    ee24aa32_sequential_read(driver, (address >> 8) & 0xFF, address & 0xFF, data, len)


int ee24aa32_sequential_write(i2c_driver_t driver, uint8_t block, uint8_t address, uint8_t *data, size_t len);
int ee24aa32_sequential_read(i2c_driver_t driver, uint8_t block, uint8_t address, uint8_t *data, size_t len);
int ee24aa32_wait_for_cache_flush(i2c_driver_t driver, size_t bytes_written);

#endif
