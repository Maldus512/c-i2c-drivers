#ifndef SHTC3_H_INCLUDED
#define SHTC3_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define SHTC3_DEFAULT_ADDRESS (0x70 << 1)

#define SHTC3_NORMAL_MEASUREMENT_PERIOD_MS 13


int shtc3_sleep(i2c_driver_t driver);
int shtc3_wakeup(i2c_driver_t driver);
int shtc3_reset(i2c_driver_t driver);
int shtc3_start_temperature_humidity_measurement(i2c_driver_t driver);
int shtc3_read_temperature_humidity_measurement(i2c_driver_t driver, int16_t *temperature, int16_t *humidity);


#endif