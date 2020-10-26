#ifndef M41T81_H_INCLUDED
#define M41T81_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"

#define M41T11_DEFAULT_ADDRESS 0xD0
#define SEG_TIME               0x01


int M41T81_init(i2c_driver_t driver);
int m41t81_set_time(i2c_driver_t driver, const rtc_time_t *pTime);
int m41t81_get_time(i2c_driver_t driver, rtc_time_t *pTime);

#if 0
int IsSecIncr (rtc_time_t *pTime);
int StartClock (void);
int ResyncClock (void);
void DoClock (rtc_time_t *pTime);
int WriteRTCRAM (const void *pData, int nLen);
int ReadRTCRAM (void *pData, int nLen);

unsigned char Check_TIME (rtc_time_t *);
void calc_day_of_week (rtc_time_t *pTime);
void ora_legale (rtc_time_t *pTime);
void ora_legale_clock_adjust (rtc_time_t *pTime, unsigned char direction_flag);
void Set_ora_legale_auto_adjust (unsigned char ora_legale_flag);



int rtccmp(rtc_time_t t1, rtc_time_t t2);

// void Display_temperatura (void);

void Display_time (void);
void Display_date (void);
void Display_day (void);

void Get_Str_Curr_Time (void);
void Get_Str_Prog_Time (void);
void Get_Str_From_Time (rtc_time_t time, unsigned char *str_raw, unsigned char *str_time, unsigned char *str_date);
void Get_Str_Prog_From_Time (rtc_time_t time, unsigned char *str_prog);
#endif

#endif