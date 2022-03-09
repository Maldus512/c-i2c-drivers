#include <string.h>

#include "../../../i2c_common/i2c_common.h"

#include "m41t81.h"

#define BCD2BIN(x) ((((x) >> 4) & 0xF) * 10 + ((x)&0xF))
#define BIN2BCD(x) ((((x) / 10) << 4) | ((x) % 10))
#define STOP_BIT   0x80

static volatile unsigned char ora_legale_auto_adjust_flag;
static volatile unsigned char cPrevSec;
static volatile short         nMsec;
static volatile short         nSec;
static volatile short         nMin;
static volatile short         nHour;
static volatile short         nDayOfWeek;
static volatile short         nDay;
static volatile short         nMonth;
static volatile short         nYear;

const int nMonthOfYear[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


extern unsigned char stringa[64];


unsigned char str_curr_time[13]   = "0000000000000";
unsigned char str_date_display[9] = "GG/MM/AA";
unsigned char str_time_display[9] = "hh:mm:ss";
unsigned char str_prog_time[13]   = "0000000000000";

rtc_time_t CurrTime; /* orologio di sitema */
rtc_time_t ProgTime; /* orologio di sitema programmato */




static void BCD_to_ASCII(char BCD, char *string) {
    *string = (((unsigned int)BCD & 0xf0) >> 4) | 0x30;

    if (*string < '0' || *string > '9') {
        *string = '0';
    }
    string++;

    *string = ((BCD & 0x0f)) | 0x30;

    if (*string < '0' || *string > '9') {
        *string = '0';
    }
}


/*----------------------------------------------------------------------------*/
/* calc_day_of_week                                                           */
/*----------------------------------------------------------------------------*/
static void calc_day_of_week(rtc_time_t *pTime) {
    unsigned int calc_a, calc_y, calc_m, calc_d;

    // Formula to calculate the day-of-week:
    //
    // a= (14-month)/12
    // y= year - a
    // m= (month+(12*a))-2
    // d= (day+y+(y/4)-(y/100)+(y/400)+((31*m)/12)) mod 7
    //
    // Remember: "mod 7" simply means to take the remainder, as in %7 in C
    //
    // The following variables are assigned:
    //
    // nDayOfWeek       Current day of the week (1-7, 1=Monday)
    // pTime->cDate      Current Day
    // pTime->cMonth    Current month (1-12)
    // pTime->cYear     Current year (00-99) Must add 2000

    calc_a = (14 - BCD2BIN(pTime->month)) / 12;
    calc_y = (BCD2BIN(pTime->month) + 2000) - calc_a;
    calc_m = (BCD2BIN(pTime->month) + (12 * calc_a)) - 2;
    calc_d =
        (BCD2BIN(pTime->month) + calc_y + (calc_y / 4) - (calc_y / 100) + (calc_y / 400) + ((31 * calc_m) / 12)) % 7;

    // The day of week now resides in the calc_d variable.
    // Remember, the day-of-week calculated is in the range 0-6,
    // with Sunday = 0. We want Sunday = 7.

    if (calc_d) {
        // Non-zero - store day-of-week 1-6
        pTime->day = calc_d;
    } else {
        // Equal to zero - this is Sunday - reset to 7
        pTime->day = 7;
    }
}




/*----------------------------------------------------------------------------*/
/* Init_RTC                                                                   */
/*----------------------------------------------------------------------------*/
int m41t81_init(i2c_driver_t driver) {
    uint8_t    seconds;
    rtc_time_t rtc_time;
    int        res;

    cPrevSec = 0;

    // controlla stop bit (oscillator stopped)
    if ((res = i2c_read_register(driver, SEG_TIME, &seconds, 1)))
        return res;

    if (seconds & STOP_BIT) {
        // inizializza default
        rtc_time.sec  = 0;
        rtc_time.min  = 20;
        rtc_time.hour = 12;

        rtc_time.wday = 4;

        rtc_time.day   = 4;
        rtc_time.month = 9;
        rtc_time.year  = 20;

        if ((res = m41t81_set_time(driver, rtc_time)))
            return res;
    }

    // azzera halt update bit
    if ((res = i2c_read_register(driver, SEG_TIME + 11, &seconds, 1)))
        return res;

    seconds &= ~'\x40';

    if ((res = i2c_write_register(driver, SEG_TIME + 11, &seconds, 1)))
        return res;

    if ((res = i2c_read_register(driver, SEG_TIME + 11, &seconds, 1)))
        return res;

    if (seconds & '\x40')
        return -1;

    return 0;
}


int m41t81_set_time(i2c_driver_t driver, rtc_time_t rtc_time) {
    // calc_day_of_week((rtc_time_t *)pTime);
    uint8_t buffer[7] = {
        rtc_time.sec, rtc_time.min, rtc_time.hour, rtc_time.day, rtc_time.wday, rtc_time.month, rtc_time.year,
    };

    for (size_t i = 0; i < sizeof(buffer); i++) {
        buffer[i] = BIN2BCD(buffer[i]);
    }

    return i2c_write_register(driver, SEG_TIME, buffer, sizeof(buffer));
}


int m41t81_get_time(i2c_driver_t driver, rtc_time_t *rtc_time) {
    int err;
    if (rtc_time == NULL) {
        return -1;
    }

    uint8_t buffer[7] = {0};

    if ((err = i2c_read_register(driver, SEG_TIME, buffer, sizeof(buffer)))) {
        return err;
    }

    rtc_time->sec   = BCD2BIN((buffer[0] & (~STOP_BIT)));
    rtc_time->min   = BCD2BIN(buffer[1]);
    rtc_time->hour  = BCD2BIN(buffer[2]);
    rtc_time->day   = BCD2BIN(buffer[3]);
    rtc_time->wday  = BCD2BIN(buffer[4]);
    rtc_time->month = BCD2BIN(buffer[5]);
    rtc_time->year  = BCD2BIN(buffer[6]);

    return 0;
}

#if 0

int rtccmp(rtc_time_t t1, rtc_time_t t2) {

    if (t1.cSec != t2.cSec) {
        return 1;
    }
    if (t1.cMin != t2.cMin) {
        return 1;
    }
    if (t1.cHour != t2.cHour) {
        return 1;
    }
    if (t1.cDay != t2.cDay) {
        return 1;
    }
    if (t1.cDate != t2.cDate) {
        return 1;
    }
    if (t1.cMonth != t2.cMonth) {
        return 1;
    }
    if (t1.cYear != t2.cYear) {
        return 1;
    }
    if (t1.cCtrl != t2.cCtrl) {
        return 1;
    }

    return 0;
}






/*----------------------------------------------------------------------------*/
/* IsSecIncr                                                                  */
/*----------------------------------------------------------------------------*/
int IsSecIncr (rtc_time_t *pTime)
{
    unsigned char cSec;
    
//     if (!I2C_Read (M41T11_ADDR, SEG_TIME, &cSec, 1))
    if (!I2C_read_register (M41T11_ADDR, SEG_TIME, &cSec, 1))
    {
        return FALSE;
    }
    
    if (cPrevSec == cSec)
    {
        return FALSE;
    }
    
    cPrevSec = cSec;
    
    if (pTime != NULL)
    {
//         return I2C_Read (M41T11_ADDR, SEG_TIME, (unsigned char*) pTime, sizeof (rtc_time_t));
        return I2C_read_register (M41T11_ADDR, SEG_TIME, (unsigned char*) pTime, sizeof (rtc_time_t));
    }
    return TRUE;
}



/*----------------------------------------------------------------------------*/
/* Check_TIME                                                                 */
/*----------------------------------------------------------------------------*/
unsigned char Check_TIME (rtc_time_t *pTime)
{
    int nDayOfMonth = 0;
    int nYear = 0;
    int month = 0;
    unsigned char flag = FALSE;
    
    if (BCD2BIN(pTime->cSec)> 59)
    {
        pTime->cSec = 0;
        flag = TRUE;
    }
    
    // minuti
    if (BCD2BIN(pTime->cMin)> 59)
    {
        pTime->cMin = 0;
        flag = TRUE;
    }
    
    // ore
    if (BCD2BIN(pTime->cHour)> 23)
    {
        pTime->cHour = 0;
        flag = TRUE;
    }
    
    // giorno della settimana
    if (nDayOfWeek > 7)
    {
        pTime->cDay  = 1;
        flag = TRUE;
    }
    
    // giorno del mese
    if (BCD2BIN(pTime->cDate)> 31 ||  BCD2BIN(pTime->cDate)== 0)
    {
        pTime->cDate = 1;
        flag = TRUE;
    }
    
    nDayOfMonth = nMonthOfYear[BCD2BIN(pTime->cDate) - 1];
    nYear = (unsigned char)BIN2BCD(pTime->cYear);//(unsigned char)BIN2BCD(nYear % 100);
    month = BCD2BIN(pTime->cMonth);
    
    if (month == 2)
    {
        // anno bisestile ?
        if ((nYear %4 == 0 && nYear %100 != 0) || (nYear %400 == 0))
        {
            if (BCD2BIN(pTime->cDate) > 29)
            {
                pTime->cDate = BIN2BCD(29);
                flag = TRUE;
            }
        }
        else
        {
            if (BCD2BIN(pTime->cDate) > 28)
            {
                pTime->cDate = BIN2BCD(28);
                flag = TRUE;
            }
        }
    }
    else
    {  
        if (month == 4 || month == 6 || month == 9 || month == 11)
        {
            if (BCD2BIN(pTime->cDate) > 30)
            {
                pTime->cDate = BIN2BCD(30);
                flag = TRUE;
            }
        }
        else
        {
            if (BCD2BIN(pTime->cDate) > 31)
            {
                pTime->cDate = BIN2BCD(31);
                flag = TRUE;
            }
        }
    }
    
    if (BCD2BIN(pTime->cMonth) > 12 || BCD2BIN(pTime->cMonth) == 0)
    {
        pTime->cMonth = 1;
        flag = TRUE;
    }
    return(flag);
}








/*----------------------------------------------------------------------------*/
/* ora_legale                                                                 */
/*----------------------------------------------------------------------------*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>> FUNCTION: ora_legale() <<<<<<<<<<<<<<<<<<<<<<<<<<*/
/* Purpose: Check for Daylight Saving Time.                                   */
/*                                                                            */
/* Notes:  To calculate daylight saving time, it will always begin the first  */
/*      Sunday in April, and end the last Sunday in October. We can check     */
/*      to see if we have the first Sunday by testing for the first           */
/*      occurance of a "7" day-of-week.                                       */
/*      The time will change at 2:00am. A flag will be set to indicate the    */
/*      time is in DST and will need to be incremented in October.            */
/*                                                                            */
/*      If enabled, the daylight saving change will only occur if the         */
/*      system is powered up and actually "sees" the transition to 2:00am     */
/*      on the first Sunday in April and the last Sunday in October.          */
/*                                                                            */
/*      If the user changes the date to jump into or out-of daylight          */
/*      saving time, the system will not automatically adjust the time.       */
/*      If a manual change to the date is made, the system will assume a      */
/*      manual change to the time is required also.                           */
/*                                                                            */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
void ora_legale (rtc_time_t *pTime)
{
    // Flags tested:
    //
    // dst_auto_adjust_flag:
    // Set if we need to automatically adjust for daylight saving.
    
    if(ora_legale_auto_adjust_flag)
    {
    // Daylight Saving Time auto adjust is enabled - Check to see if we need
    // to adjust the time. First Sunday in April, and Last Sunday in October.
        
        //*** APRIL - SPRING FORWARD - ADD 1 HOUR ****
        if(BCD2BIN(pTime->cMonth) == 3)
        {
            // We are in march - see if this is the last Sunday
            if(BCD2BIN(pTime->cDate) >= 25)
            {
            // We are in the first week of April - see which day is equal
            // to the 7th day of the week (Sunday)
                if(BCD2BIN(pTime->cDay) == 7)
                {
                    // Its march, we are currently in the first Sunday, see if
                    // its 2:00am - Adjust the time forward when
                    // 2:00am arrives.
                    if(BCD2BIN(pTime->cHour) == 2)
                    {
                        if(BCD2BIN(pTime->cMin) == 0)
                        {
                            if(BCD2BIN(pTime->cSec) == 0)
                            {
                                // Its 2:00am - increment the hour and set the
                                // dst_active_flag
                                // Call the ora_legale_clock_adjust() function 
                                // to add one hour.
                                ora_legale_clock_adjust(pTime,1);
                            }
                        }
                    }
                }
            }
        }
        
        //*** OCTOBER - FALL BACK - SUBTRACT 1 HOUR ****
        if(BCD2BIN(pTime->cMonth) == 10)
        {
            // We are in October - see if this is the last Sunday
            if(BCD2BIN(pTime->cDate) >= 25)
            {
                // We are in the last week of October - see which day
                // is equal to the last Sunday.
                if(BCD2BIN(pTime->cDay) == 7)
                {
                    // Its October, we are currently in the last Sunday, see if
                    // its 2:00am - Adjust the time backwards when 2:00am
                    // arrives.
                    if(BCD2BIN(pTime->cHour) == 3)
                    {
                        if(BCD2BIN(pTime->cMin) == 0)
                        {
                            if(BCD2BIN(pTime->cSec) == 0)
                            {
                                // Its 3:00am - decrement the hour and set the
                                // dst_active_flag
                                // Call the ora_legale_clock_adjust() function
                                // to add one hour.
                                ora_legale_clock_adjust(pTime,0);
                            }
                        }
                    }
                }
            }
        }       // end October check
    }
}






/*----------------------------------------------------------------------------*/
/* ora_legale_clock_adjust                                                    */
/*----------------------------------------------------------------------------*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>> FUNCTION: ora_legale_clock_adjust() <<<<<<<<<<<<<*/
/* Purpose: Adjust the clock back/ahead an hour.                              */
/*                                                                            */
/*   Notes:  This routine is used to add or subtract an hour from the system  */
/*      clock (used for daylight saving time).  A "direction flag" will       */
/*      be passed to this routine to indicate an addition or subtraction      */
/*      to the current system time.                                           */
/*           direction_flag: 0= subtract, 1 = add.                            */
/*                                                                            */
/*      This routine can be called anytime the user resets the date.          */
/*      (it may not always change the time at 2:00am - if the user changes    */
/*      the date to jump inside/outside of daylight saving time, the clock    */
/*      should automatically adjust (if dst is enabled)                       */
/*                                                                            */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
void ora_legale_clock_adjust (rtc_time_t *pTime, unsigned char direction_flag)
{
    // Direction flag: 0=subtract, 1=add
    if(direction_flag)
    {
        // Non-Zero - must be increment
        // Make sure we will not cross the 23:00 to 0:00 boundary
        if(BCD2BIN(pTime->cHour) == 23)
        {
            // Set for 0:00
            pTime->cHour = (unsigned char) BIN2BCD (0);
        }
        else
        {
            // Normal increment
            pTime->cHour++;
        }
    }
    else
    {
        // direction flag = 0, subtract an hour from the current time
        // Make sure we will not cross the 0:00 to 23:00 boundry
        if(BCD2BIN(pTime->cHour) == 0)
        {
            // Set for 23:00
            pTime->cHour = (unsigned char) BIN2BCD (23);
        }
        else
        {
            // Normal decrement
            pTime->cHour--;
        }
    }
    SetTime(pTime);
}

/*----------------------------------------------------------------------------*/
/* Set_ora_legale_clock_adjust                                                */
/*----------------------------------------------------------------------------*/
void Set_ora_legale_auto_adjust (unsigned char ora_legale_flag)
{
    ora_legale_auto_adjust_flag = ora_legale_flag;
}


/*----------------------------------------------------------------------------*/
/*  Get_Str_Curr_Time (void)                                                  */
/*----------------------------------------------------------------------------*/
void Get_Str_Curr_Time (void)
{
#ifndef FREE_INT
    INTCON2bits.GIE = 0;
#endif
    Get_Str_From_Time(CurrTime, str_curr_time, str_time_display, str_date_display);
#ifndef FREE_INT
    INTCON2bits.GIE = 1;
#endif

}



/*----------------------------------------------------------------------------*/
/*  Get_Str_Prog_Time (void)                                                  */
/*----------------------------------------------------------------------------*/
void Get_Str_Prog_Time (void)
{
    Get_Str_Prog_From_Time(ProgTime, str_prog_time);
}

void Get_Str_Prog_From_Time (rtc_time_t time, unsigned char *str_prog)
{
    memcpy (&str_prog[0], "AAMMGGHHMMSS", 12);
    BCD_to_ASCII (time.cYear, (char *)&str_prog[0]);
    BCD_to_ASCII (time.cMonth, (char *)&str_prog[2]);
    BCD_to_ASCII (time.cDate, (char *)&str_prog[4]);
    BCD_to_ASCII (time.cHour, (char *)&str_prog[6]);
    BCD_to_ASCII (time.cMin, (char *)&str_prog[8]);
    BCD_to_ASCII (time.cSec, (char *)&str_prog[10]);
    str_prog[12] = 0;
}



void Get_Str_From_Time (rtc_time_t time, unsigned char *str_raw, unsigned char *str_time, unsigned char *str_date)
{
    memcpy (&str_raw[0], "AAMMGGHHMMSS", 12);
    BCD_to_ASCII (time.cYear, (char *)&str_raw[0]);
    BCD_to_ASCII (time.cMonth, (char *)&str_raw[2]);
    BCD_to_ASCII (time.cDate, (char *)&str_raw[4]);
    BCD_to_ASCII (time.cHour, (char *)&str_raw[6]);
    BCD_to_ASCII (time.cMin, (char *)&str_raw[8]);
    BCD_to_ASCII (time.cSec, (char *)&str_raw[10]);
    str_raw[12] = 0;
    
    if (str_date != NULL) {
        str_date[0] = str_raw [4];
        str_date[1] = str_raw [5];
        str_date[2] = '/';
        str_date[3] = str_raw [2];
        str_date[4] = str_raw [3];
        str_date[5] = '/';
        str_date[6] = str_raw [0];
        str_date[7] = str_raw [1];
        str_date[8] = 0;
    }
    
    if (str_time != NULL) {
        str_time[0] = str_raw [6];
        str_time[1] = str_raw [7];
        str_time[2] = ':';
        str_time[3] = str_raw [8];
        str_time[4] = str_raw [9];
        str_time[5] = ':';
        str_time[6] = str_raw [10];
        str_time[7] = str_raw [11];
        str_time[8] = 0;
    }
}

#endif