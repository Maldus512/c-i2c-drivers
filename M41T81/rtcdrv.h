/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: RtcDrv.h                                                          */
/*                                                                            */
/*      gestione RTC M41T81                                                   */
/*                                                                            */
/*  Autore: Massimo Zanna                                                     */
/*                                                                            */
/*  Data  : 20/01/2003      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 21/05/2016      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#define BCD2BIN(x) ((((x) >> 4) & 0xF) * 10 + ((x) & 0xF))
#define BIN2BCD(x) ((((x) / 10) << 4) | ((x) % 10))

#define M41T11_ADDR     '\xD0'
#define SEG_TIME        '\x01'





typedef struct _RTC_TIME
{
    unsigned char cSec;
    unsigned char cMin;
    unsigned char cHour;
    unsigned char cDay;
    unsigned char cDate;
    unsigned char cMonth;
    unsigned char cYear;
    unsigned char cCtrl;
}RTC_TIME;

int RTC_Init (void);
void InitRTCport (void);

int SetTime (const RTC_TIME *pTime);
int GetTime (RTC_TIME *pTime);

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





// void Display_temperatura (void);

void Display_time (void);
void Display_date (void);
void Display_day (void);

void Get_Str_Curr_Time (void);
void Get_Str_Prog_Time (void);
void Get_Str_From_Time (RTC_TIME time, unsigned char *str_raw, unsigned char *str_time, unsigned char *str_date);
void Get_Str_Prog_From_Time (RTC_TIME time, unsigned char *str_prog);



void BCD_to_ASCII (char BCD, char *string);
void ASCII_to_BCD (char *string, char BCD);
