#include <string.h>
#include "../../../i2c_common/i2c_common.h"
#include "ds1307.h"
#ifdef I2C_DEVICES_STRUCT_TM_CONVERSION
#include <time.h>
#endif


#define SECONDS_REGISTER 0x00
#define CLOCK_HALT_BIT   0x80


int ds1307_is_clock_halted(i2c_driver_t driver) {
    uint8_t readbuf[1]  = {0};
    uint8_t writebuf[1] = {SECONDS_REGISTER};

    int res =
        driver.i2c_transfer(driver.device_address, writebuf, sizeof(writebuf), readbuf, sizeof(readbuf), driver.arg);
    if (res) {
        return res;
    } else {
        return (readbuf[0] & CLOCK_HALT_BIT) > 0;
    }
}


int ds1307_get_time(i2c_driver_t driver, rtc_time_t *rtc_time) {
    uint8_t readbuf[7]  = {0};
    uint8_t writebuf[1] = {SECONDS_REGISTER};

    int res =
        driver.i2c_transfer(driver.device_address, writebuf, sizeof(writebuf), readbuf, sizeof(readbuf), driver.arg);
    if (res) {
        return res;
    }

    rtc_time->sec   = BCD2BIN(readbuf[0]);
    rtc_time->min   = BCD2BIN(readbuf[1]);
    rtc_time->hour  = BCD2BIN(readbuf[2]);
    rtc_time->wday  = BCD2BIN(readbuf[3]);
    rtc_time->day   = BCD2BIN(readbuf[4]);
    rtc_time->month = BCD2BIN(readbuf[5]);
    rtc_time->year  = BCD2BIN(readbuf[6]);

    return 0;
}


int ds1307_set_time(i2c_driver_t driver, rtc_time_t rtc_time, int halted) {
    uint8_t writebuf[8] = {0};

    writebuf[0] = SECONDS_REGISTER;
    writebuf[1] = BIN2BCD(rtc_time.sec) & (halted ? CLOCK_HALT_BIT : 0);
    writebuf[2] = BIN2BCD(rtc_time.min);
    writebuf[3] = BIN2BCD(rtc_time.hour);
    writebuf[4] = BIN2BCD(rtc_time.wday);
    writebuf[5] = BIN2BCD(rtc_time.day);
    writebuf[6] = BIN2BCD(rtc_time.month);
    writebuf[7] = BIN2BCD(rtc_time.year);

    return driver.i2c_transfer(driver.device_address, writebuf, sizeof(writebuf), NULL, 0, driver.arg);
}


#ifdef I2C_DEVICES_STRUCT_TM_CONVERSION
rtc_time_t ds1307_rtc_from_tm(struct tm tm) {
    rtc_time_t res = {
        .sec   = tm.tm_sec,
        .min   = tm.tm_min,
        .hour  = tm.tm_hour,
        .day   = tm.tm_mday,
        .month = tm.tm_mon + 1,
        .year  = tm.tm_year - 100,
        .wday  = tm.tm_wday + 1,
    };
    return res;
}


struct tm ds1307_tm_from_rtc(rtc_time_t rtc) {
    struct tm res = {
        .tm_sec  = rtc.sec,
        .tm_min  = rtc.min,
        .tm_hour = rtc.hour,
        .tm_mday = rtc.day,
        .tm_mon  = rtc.month - 1,
        .tm_wday = rtc.wday - 1,
        .tm_year = rtc.year + 100,
    };
    return res;
}
#endif