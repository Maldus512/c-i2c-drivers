#include "../../../i2c_common/i2c_common.h"
#include "rx8010.h"
#ifdef I2C_DEVICES_STRUCT_TM_CONVERSION
#include <time.h>
#endif

#define REGISTER_SEC     0x10
#define REGISTER_MIN     0x11
#define REGISTER_HOUR    0x12
#define REGISTER_WEEK    0x13
#define REGISTER_DAY     0x14
#define REGISTER_MONTH   0x15
#define REGISTER_YEAR    0x16
#define REGISTER_FLAG    0x1E
#define REGISTER_CONTROL 0x1F

#define BIT_REGISTER_CONTROL_STOP 0x40


int rx8010_is_stopped(i2c_driver_t driver) {
    uint8_t write_buffer[] = {REGISTER_CONTROL};
    uint8_t read_buffer[1] = {0};

    int res = driver.i2c_transfer(driver.device_address, write_buffer, sizeof(write_buffer), read_buffer,
                                  sizeof(read_buffer), driver.arg);
    if (res) {
        return res;
    }

    return (read_buffer[0] & BIT_REGISTER_CONTROL_STOP) > 0;
}


int rx8010_set_stop(i2c_driver_t driver, int stop) {
    uint8_t write_buffer[] = {REGISTER_CONTROL};
    uint8_t read_buffer[1] = {0};

    int res = driver.i2c_transfer(driver.device_address, write_buffer, sizeof(write_buffer), read_buffer,
                                  sizeof(read_buffer), driver.arg);
    if (res) {
        return res;
    }

    if (((read_buffer[0] & BIT_REGISTER_CONTROL_STOP) > 0) == (stop > 0)) {
        return 0;
    }

    if (stop) {
        read_buffer[0] |= BIT_REGISTER_CONTROL_STOP;
    } else {
        read_buffer[0] &= ~BIT_REGISTER_CONTROL_STOP;
    }

    uint8_t overwrite_buffer[] = {REGISTER_CONTROL, read_buffer[0]};
    return driver.i2c_transfer(driver.device_address, overwrite_buffer, sizeof(overwrite_buffer), NULL, 0, driver.arg);
}


int rx8010_get_time(i2c_driver_t driver, rtc_time_t *rtc_time) {
    uint8_t read_buffer[7]  = {0};
    uint8_t write_buffer[1] = {REGISTER_SEC};

    int res = driver.i2c_transfer(driver.device_address, write_buffer, sizeof(write_buffer), read_buffer,
                                  sizeof(read_buffer), driver.arg);
    if (res) {
        return res;
    }

    rtc_time->sec   = BCD2BIN(read_buffer[0]);
    rtc_time->min   = BCD2BIN(read_buffer[1]);
    rtc_time->hour  = BCD2BIN(read_buffer[2]);
    rtc_time->wday  = BCD2BIN(read_buffer[3]);
    rtc_time->day   = BCD2BIN(read_buffer[4]);
    rtc_time->month = BCD2BIN(read_buffer[5]);
    rtc_time->year  = BCD2BIN(read_buffer[6]);

    return 0;
}


int rx8010_set_time(i2c_driver_t driver, rtc_time_t rtc_time) {
    uint8_t writebuf[8] = {0};

    writebuf[0] = REGISTER_SEC;
    writebuf[1] = BIN2BCD(rtc_time.sec);
    writebuf[2] = BIN2BCD(rtc_time.min);
    writebuf[3] = BIN2BCD(rtc_time.hour);
    writebuf[4] = BIN2BCD(rtc_time.wday);
    writebuf[5] = BIN2BCD(rtc_time.day);
    writebuf[6] = BIN2BCD(rtc_time.month);
    writebuf[7] = BIN2BCD(rtc_time.year);

    return driver.i2c_transfer(driver.device_address, writebuf, sizeof(writebuf), NULL, 0, driver.arg);
}


#ifdef I2C_DEVICES_STRUCT_TM_CONVERSION
rtc_time_t rx8010_rtc_from_tm(struct tm tm) {
    rtc_time_t res = {
        .sec   = tm.tm_sec,
        .min   = tm.tm_min,
        .hour  = tm.tm_isdst ? tm.tm_hour - 1 : tm.tm_hour,
        .day   = tm.tm_mday,
        .month = tm.tm_mon + 1,
        .year  = tm.tm_year - 100,
        .wday  = tm.tm_wday + 1,
    };
    return res;
}


struct tm rx8010_tm_from_rtc(rtc_time_t rtc) {
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