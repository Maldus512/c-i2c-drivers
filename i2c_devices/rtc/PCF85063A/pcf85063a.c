#include <string.h>

#include "../../../i2c_common/i2c_common.h"
#include "pcf85063a.h"

#define REG_CTRL1    0
#define REG_CTRL2    1
#define REG_OFFSET   2
#define REG_RAM_BYTE 3
#define REG_SEC      4
#define REG_MIN      5
#define REG_HOUR     6
#define REG_DAY      7
#define REG_WDAY     8
#define REG_MONTH    9
#define REG_YEAR     10

#define SEG_TIME  0x01
#define STOP_BIT  (1 << 5)
#define RESET_BIT (1 << 4)


static int BCD_to_int(char bcd, uint8_t tens_len) {
    int r = 0;
    r += bcd & 0x0f;
    r += ((bcd >> 4) & ((1 << tens_len) - 1)) * 10;
    return r;
}


static uint8_t int_to_bcd(int val, uint8_t tens_len) {
    char c = 0;
    int  i;
    for (i = 0; i < 4; i++) {
        c |= (val % 10) & (1 << i);
    }
    for (i = 0; i < 4; i++) {
        c |= ((val / 10) & (1 << i)) << 4;
    }
    return c & ((1 << (tens_len + 4)) - 1);
}


static int check_clock_integrity(i2c_driver_t driver) {
    uint8_t val;
    int     res;
    if ((res = i2c_read_register(driver, REG_SEC, &val, 1))) {
        return res;
    }
    return val & 0x80;
}


/*----------------------------------------------------------------------------*/
/* Init_RTC                                                                   */
/*----------------------------------------------------------------------------*/
int pcf85063a_init(i2c_driver_t driver, uint8_t cap_size) {
    // unsigned char cData;
    // rtc_time_t tCurrTime;
    int     res;
    uint8_t val;

    // check if frozen
    if ((res = i2c_read_register(driver, REG_CTRL1, &val, 1)))
        return res;

    // add cap_size information to value written to register
    val &= ~(PCF85063A_CAPACITOR_7PF);
    val |= cap_size;
    if ((res = i2c_write_register(driver, REG_CTRL1, &val, 1)))
        return res;

    // if clock integrity has been compromised, reset time to a default value
    if (check_clock_integrity(driver)) {
        rtc_time_t pTime;
        pTime.sec   = 0;
        pTime.min   = 0;
        pTime.hour  = 0;
        pTime.day   = 0;
        pTime.wday  = 0;
        pTime.month = 0;
        pTime.year  = 20;

        pcf85063a_set_time(driver, &pTime);
    }

    return 0;
}


int pcf85063a_set_time(i2c_driver_t driver, const rtc_time_t *pTime) {
    uint8_t tmp;
    if (pTime == NULL)
        return -1;

    tmp = int_to_bcd(pTime->sec, 3);
    i2c_write_register(driver, REG_SEC, &tmp, sizeof(uint8_t));
    tmp = int_to_bcd(pTime->min, 3);
    i2c_write_register(driver, REG_MIN, &tmp, sizeof(uint8_t));
    tmp = int_to_bcd(pTime->hour, 2);
    i2c_write_register(driver, REG_HOUR, &tmp, sizeof(uint8_t));
    tmp = int_to_bcd(pTime->day, 2);
    i2c_write_register(driver, REG_DAY, &tmp, sizeof(uint8_t));
    tmp = int_to_bcd(pTime->wday, 0);
    i2c_write_register(driver, REG_WDAY, &tmp, sizeof(uint8_t));
    tmp = int_to_bcd(pTime->month, 1);
    i2c_write_register(driver, REG_MONTH, &tmp, sizeof(uint8_t));
    tmp = int_to_bcd(pTime->year, 4);
    i2c_write_register(driver, REG_YEAR, &tmp, sizeof(uint8_t));

    return 0;
}


int pcf85063a_get_time(i2c_driver_t driver, rtc_time_t *pTime) {
    uint8_t tmp;
    if (pTime == NULL)
        return -1;
    i2c_read_register(driver, REG_SEC, &tmp, sizeof(uint8_t));
    pTime->sec = BCD_to_int(tmp, 3);
    i2c_read_register(driver, REG_MIN, &tmp, sizeof(uint8_t));
    pTime->min = BCD_to_int(tmp, 3);
    i2c_read_register(driver, REG_HOUR, &tmp, sizeof(uint8_t));
    pTime->hour = BCD_to_int(tmp, 2);
    i2c_read_register(driver, REG_DAY, &tmp, sizeof(uint8_t));
    pTime->day = BCD_to_int(tmp, 2);
    i2c_read_register(driver, REG_WDAY, &tmp, sizeof(uint8_t));
    pTime->wday = BCD_to_int(tmp, 0);
    i2c_read_register(driver, REG_MONTH, &tmp, sizeof(uint8_t));
    pTime->month = BCD_to_int(tmp, 1);
    i2c_read_register(driver, REG_YEAR, &tmp, sizeof(uint8_t));
    pTime->year = BCD_to_int(tmp, 4);
    return 0;
}


#ifdef I2C_DEVICES_STRUCT_TM_CONVERSION
rtc_time_t pcf85063a_rtc_from_tm(struct tm tm) {
    rtc_time_t res = {
        .sec   = tm.tm_sec,
        .min   = tm.tm_min,
        .hour  = tm.tm_hour,
        .day   = tm.tm_mday,
        .month = tm.tm_mon + 1,
        .year  = tm.tm_year % 100,
        .wday  = tm.tm_wday,
    };
    return res;
}


struct tm pcf85063a_tm_from_rtc(rtc_time_t rtc) {
    struct tm res = {
        .tm_sec  = rtc.sec,
        .tm_min  = rtc.min,
        .tm_hour = rtc.hour,
        .tm_mday = rtc.day,
        .tm_mon  = rtc.month - 1,
        .tm_wday = rtc.wday,
        .tm_year = rtc.year + 100,
    };
    return res;
}
#endif