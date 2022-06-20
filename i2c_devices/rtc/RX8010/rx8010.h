#ifndef RX8010_H_INCLUDED
#define RX8010_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define RX8010_DEFAULT_ADDRESS 0x64


int rx8010_is_stopped(i2c_driver_t driver);
int rx8010_set_stop(i2c_driver_t driver, int stop);
int rx8010_set_time(i2c_driver_t driver, rtc_time_t rtc_time);
int rx8010_get_time(i2c_driver_t driver, rtc_time_t *rtc_time);


#ifdef I2C_DEVICES_STRUCT_TM_CONVERSION
#include <time.h>

rtc_time_t rx8010_rtc_from_tm(struct tm tm);
struct tm  rx8010_tm_from_rtc(rtc_time_t rtc);
#endif


#endif