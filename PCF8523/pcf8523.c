
/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: RtcDrv.c                                                          */
/*                                                                            */
/*      gestione RTC PCF8523                                                   */
/*                                                                            */
/*  Autore: Massimo Zanna                                                     */
/*                                                                            */
/*  Data  : 20/01/2003      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 12/04/2018      REV  : 01.3                                       */
/*                                                                            */
/******************************************************************************/

#include <string.h>

#include "i2c_common/i2c_common.h"

#include "pcf8523.h"

#define BCD2BIN(x) ((((x) >> 4) & 0xF) * 10 + ((x)&0xF))
#define BIN2BCD(x) ((((x) / 10) << 4) | ((x) % 10))

int BCD_to_int(char bcd, uint8_t tens_len)
{
    int r = 0;
    r += bcd & 0x0f;
    r += ((bcd >> 4) & ((1 << tens_len) - 1)) * 10;
    return r;
}

uint8_t int_to_bcd(int val, uint8_t tens_len)
{
    char c = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        c |= (val % 10) & (1 << i);
    }
    for (i = 0; i < 4; i++)
    {
        c |= ((val / 10) & (1 << i)) << 4;
    }
    return c & ((1 << (tens_len + 4)) - 1);
}

int check_clock_integrity(i2c_driver_t driver)
{
    uint8_t val;
    int res;
    if ((res = i2c_read_register(driver, REG_SEC, &val, 1)))
    {
        return res;
    }
    return val & 0x80;
}

/*----------------------------------------------------------------------------*/
/* Init_RTC                                                                   */
/*----------------------------------------------------------------------------*/
int pcf8523_init(i2c_driver_t driver, uint8_t cap_size, uint8_t battery_control)
{
    // unsigned char cData;
    // RTC_TIME tCurrTime;
    int res;
    uint8_t val;

    //check if frozen
    if ((res = i2c_read_register(driver, REG_CTRL1, &val, 1)))
    {
        return res;
    }
    if (val & 0x20)
    {
        //if frozen, issue software reset
        val |= 0x10;
    }
    // add cap_size information to value written to register
    val |= cap_size;
    if ((res = i2c_write_register(driver, REG_CTRL1, &val, 1)))
    {
        return res;
    }

    //if clock integrity has been compromised, reset time to a default value
    if (check_clock_integrity(driver))
    {
        rtc_time_t pTime;
        pTime.sec = 0;
        pTime.min = 0;
        pTime.hour = 0;
        pTime.day = 0;
        pTime.wday = 0;
        pTime.month = 0;
        pTime.year = 20;

        pcf8523_set_time(driver, &pTime);
    }
    //write battery control to register control 3
    val = battery_control << 5;
    if ((res = i2c_write_register(driver, REG_CTRL3, &val, 1)))
    {
        return res;
    }
    return 0;
}

int pcf8523_set_time(i2c_driver_t driver, const RTC_TIME *pTime)
{
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

int pcf8523_get_time(i2c_driver_t driver, RTC_TIME *pTime)
{
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
