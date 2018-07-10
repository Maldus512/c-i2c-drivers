/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: I2C_drv.c                                                         */
/*                                                                            */
/*      gestione I2C                                                          */
/*                                                                            */
/*  Autore: Massimo Zanna                                                     */
/*                                                                            */
/*  Data  : 03/09/2007      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 23/05/2016      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#include "HardwareProfile.h"

#include <string.h>
#include <libpic30.h>

#include "i2c_bitbang.h"
#include "system.h"
#include "i2c_module2.h"


/*----------------------------------------------------------------------------*/
/*  Init_I2C_b                                                                */
/*----------------------------------------------------------------------------*/
void Init_I2C (void)
{
#if I2C_MODE == I2C_BITBANG
    Init_I2C_bitbang();
#elif I2C_MODE == I2C_MODULE
    Init_I2C_module();
#endif
}



void write_protect_enable() {
#ifdef WRITE_PROTECT
    WRITE_PROTECT = 1;
#endif
}

void write_protect_disable() {
#ifdef WRITE_PROTECT
    WRITE_PROTECT = 0;
#endif
}