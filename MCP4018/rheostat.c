
/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: RtcDrv.c                                                          */
/*                                                                            */
/*      gestione RTC M41T81                                                   */
/*                                                                            */
/*  Autore: Massimo Zanna                                                     */
/*                                                                            */
/*  Data  : 20/01/2003      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 12/04/2018      REV  : 01.3                                       */
/*                                                                            */
/******************************************************************************/

#include <xc.h>
#include "HardwareProfile.h"
#include "MCP4018/rheostat.h"
#include <string.h>
#include "i2c_drv.h"


#define     RHEOSTAT_ADDRESS        0x5E



int setRheostatValue(char val) {
    if (val > 127) 
        return -1;
    
    return I2C_Write_b(RHEOSTAT_ADDRESS, val, NULL, 0);
}


int getRheostatValue(char *val) {
    return I2C_CurrentRead_b(RHEOSTAT_ADDRESS,(unsigned char*) val, 1);
}