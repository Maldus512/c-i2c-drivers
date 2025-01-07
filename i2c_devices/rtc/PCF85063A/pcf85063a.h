#ifndef PCF85063A_H_INCLUDED
#define PCF85063A_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"

#define PCF85063A_DEFAULT_ADDRESS 0xA2

#define PCF85063A_CAPACITOR_12_5PF 0
#define PCF85063A_CAPACITOR_7PF    1

#define PCF85063A_LOW_DETECTION_ENABLED  000
#define PCF85063A_LOW_DETECTION_DISABLED 100

#define PCF85063A_SWITCH_DIRECT   001
#define PCF85063A_SWITCH_STANDARD 000
#define PCF85063A_SWITCH_NONE     011

#define PCF85063A_MODE_12H 1
#define PCF85063A_MODE_24H 0


int pcf85063a_init(i2c_driver_t driver, uint8_t cap_size);
int pcf85063a_set_time(i2c_driver_t driver, const rtc_time_t *pTime);
int pcf85063a_get_time(i2c_driver_t driver, rtc_time_t *pTime);


#ifdef I2C_DEVICES_STRUCT_TM_CONVERSION
#include <time.h>

rtc_time_t pcf85063a_rtc_from_tm(struct tm tm);
struct tm  pcf85063a_tm_from_rtc(rtc_time_t rtc);
#endif

#endif