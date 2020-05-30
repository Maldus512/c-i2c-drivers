/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: tcDrv.c                                                           */
/*                                                                            */
/*      gestione Serial Digital Thermal Sensor TC74                           */
/*                                                                            */
/*  Autore: Mattia MALDINI                                                    */
/*                                                                            */
/*  Data  : 16/09/2016      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 16/09/2016      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#include   <string.h>

#include "hardwareprofile.h"

#include "i2c_driver.h"
#include "TC74/tcdrv.h"


#define TEMP_REG 0
#define TC_ADDRESS 0x90
#define MAX_TEMP_DEC 2

// <editor-fold defaultstate="collapsed" desc="variables">

int temperature = 0;

char readableTemp[MAX_TEMP_LEN];

// </editor-fold>


// <editor-fold defaultstate="collapsed" desc="functions">

int readTemperature()
{
    unsigned char negative = 0, tmp = 0, count = MAX_TEMP_DEC, div = 0, i = 1, val;
    
    I2C_read_register(TEMP_REG, TC_ADDRESS, &val, 1);
    temperature = (int) val;
    negative = temperature & 0x80;
    tmp = temperature & 0x7F;
    
    while (count > 0 && i < MAX_TEMP_LEN)
    {
        div = tmp/count;
        
        if (div != 0 || (div == 0 && tmp == 0))
        {
            readableTemp[i] = div + 48;
            i++;
        }
        tmp = tmp % count;
        count = count/10;
    }
    
    readableTemp[0] = negative ? '-' : '+';
    readableTemp[(i == MAX_TEMP_LEN) ? i - 1 : i] = 0;
    
    return temperature;
}

// </editor-fold>
