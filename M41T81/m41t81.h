#ifndef M41T81_H_INCLUDED
#define M41T81_H_INCLUDED

#include "i2c_common/i2c_common.h"

#define M41T11_DEFAULT_ADDRESS 0xD0
#define SEG_TIME               0x01


typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
    uint8_t ctrl;
} rtc_time_t;

typedef rtc_time_t RTC_TIME;


int M41T81_init(i2c_driver_t driver);
int m41t81_set_time(i2c_driver_t driver, const RTC_TIME *pTime);
int m41t81_get_time(i2c_driver_t driver, RTC_TIME *pTime);

#if 0
int IsSecIncr (RTC_TIME *pTime);
int StartClock (void);
int ResyncClock (void);
void DoClock (RTC_TIME *pTime);
int WriteRTCRAM (const void *pData, int nLen);
int ReadRTCRAM (void *pData, int nLen);

unsigned char Check_TIME (RTC_TIME *);
void calc_day_of_week (RTC_TIME *pTime);
void ora_legale (RTC_TIME *pTime);
void ora_legale_clock_adjust (RTC_TIME *pTime, unsigned char direction_flag);
void Set_ora_legale_auto_adjust (unsigned char ora_legale_flag);



int rtccmp(RTC_TIME t1, RTC_TIME t2);

// void Display_temperatura (void);

void Display_time (void);
void Display_date (void);
void Display_day (void);

void Get_Str_Curr_Time (void);
void Get_Str_Prog_Time (void);
void Get_Str_From_Time (RTC_TIME time, unsigned char *str_raw, unsigned char *str_time, unsigned char *str_date);
void Get_Str_Prog_From_Time (RTC_TIME time, unsigned char *str_prog);
#endif

#endif