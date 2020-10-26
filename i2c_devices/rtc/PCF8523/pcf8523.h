#ifndef PCF8523_H_INCLUDED
#define PCF8523_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"

#define PCF8523_DEFAULT_ADDRESS 0xD0

#define PCF8523_CAPACITOR_12_5PF 0
#define PCF8523_CAPACITOR_7PF    1

#define PCF8523_LOW_DETECTION_ENABLED  000
#define PCF8523_LOW_DETECTION_DISABLED 100

#define PCF8523_SWITCH_DIRECT   001
#define PCF8523_SWITCH_STANDARD 000
#define PCF8523_SWITCH_NONE     011

#define PCF8523_MODE_12H 1
#define PCF8523_MODE_24H 0


int pcf8523_init(i2c_driver_t driver, uint8_t cap_size, uint8_t battery_control);
int pcf8523_set_time(i2c_driver_t driver, const rtc_time_t *pTime);
int pcf8523_get_time(i2c_driver_t driver, rtc_time_t *pTime);

#endif