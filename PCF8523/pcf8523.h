#ifndef PCF8523_H_INCLUDED
#define PCF8523_H_INCLUDED

#include "i2c_common/i2c_common.h"

#define PFC8523_DEFAULT_ADDRESS 0xD0
#define SEG_TIME               0x01


#define REG_CTRL1 0
#define REG_CTRL2 1
#define REG_CTRL3 2
#define REG_SEC 3
#define REG_MIN 4
#define REG_HOUR 5
#define REG_DAY 6
#define REG_WDAY 7
#define REG_MONTH 8
#define REG_YEAR 9

#define CAPACITOR_12_5PF 0
#define CAPACITOR_7PF 1

#define LOW_DETECTION_ENABLED 000
#define LOW_DETECTION_DISABLED 100

#define SWITCH_DIRECT 001
#define SWITCH_STANDARD 000
#define SWITHC_NONE 011


#define MODE_12H 1
#define MODE_24H 0
typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t wday;
    uint8_t month;
    uint8_t year;
    uint8_t ctrl;
} rtc_time_t;

typedef rtc_time_t RTC_TIME;

int pcf8523_init(i2c_driver_t driver, uint8_t cap_size, uint8_t battery_control);
int pcf8523_set_time(i2c_driver_t driver, const RTC_TIME *pTime);
int pcf8523_get_time(i2c_driver_t driver, RTC_TIME *pTime);
int BCD_to_int(char bcd, uint8_t tens_len);
uint8_t int_to_bcd(int val, uint8_t tens_len);

#endif