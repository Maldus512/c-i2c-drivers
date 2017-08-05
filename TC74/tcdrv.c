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

#include "HardwareProfile.h"

#include "MyI2C2.h"
#include "TC74/tcdrv.h"




// <editor-fold defaultstate="collapsed" desc="variables">

char temperature = 0;

unsigned char readableTemp[MAX_TEMP_LEN];

// </editor-fold>


// <editor-fold defaultstate="collapsed" desc="functions">

char readTemperature()
{
    unsigned char negative = 0, tmp = 0, count = MAX_TEMP_DEC, div = 0, i = 1;
    
    temperature = (char) I2CReadReg(TEMP_REG, TC_ADDRESS);
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
