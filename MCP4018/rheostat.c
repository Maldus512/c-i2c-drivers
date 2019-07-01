
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
#include "hardwareprofile.h"
#include "MCP4018/rheostat.h"
#include <string.h>
#include "i2c_driver.h"


#define     RHEOSTAT_ADDRESS        0x5E



int setRheostatValue(char val) {
    if (val > 127) 
        return -1;
    
    return I2C_write_register(RHEOSTAT_ADDRESS, val, NULL, 0);
}


int getRheostatValue(char *val) {
    return I2C_read_current_register(RHEOSTAT_ADDRESS,(unsigned char*) val, 1);
}