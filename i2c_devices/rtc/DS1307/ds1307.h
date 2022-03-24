#ifndef DS1307_H_INCLUDED
#define DS1307_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define DS1307_DEFAULT_ADDRESS 0xD0


int ds1307_is_clock_halted(i2c_driver_t driver);
int ds1307_get_time(i2c_driver_t driver, rtc_time_t *rtc_time);
int ds1307_set_time(i2c_driver_t driver, rtc_time_t rtc_time, int halted);


#ifdef I2C_DEVICES_STRUCT_TM_CONVERSION
#include <time.h>

rtc_time_t ds1307_rtc_from_tm(struct tm tm);
struct tm ds1307_tm_from_rtc(rtc_time_t rtc);
#endif


#endif