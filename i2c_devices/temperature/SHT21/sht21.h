#ifndef SHT21_H_INCLUDED
#define SHT21_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"

#define SHT21_DEFAULT_ADDRESS 0x80


typedef enum {
    SHT21_RESOLUTION_12_14 = 0,
    SHT21_RESOLUTION_8_12  = 0x01,
    SHT21_RESOLUTION_10_13 = 0x80,
    SHT21_RESOLUTION_11_11 = 0x81,
    SHT21_RESOLUTION_MASK  = 0x81,
} sht21_resolution_t;


int sht21_read(i2c_driver_t driver, double *temperature, double *humidity, unsigned long msdelay);
int sht21_soft_reset(i2c_driver_t driver);
int sht21_read_user_register(i2c_driver_t driver, uint8_t *ureg);
int sht21_write_user_register(i2c_driver_t driver, uint8_t ureg);
int sht21_set_resolution(i2c_driver_t driver, sht21_resolution_t resolution);

#endif