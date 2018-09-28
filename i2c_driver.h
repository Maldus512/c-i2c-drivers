/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: I2C_drv.h                                                         */
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
#if I2C_MODE == I2C_BITBANG
#include "i2c_bitbang.h"
#elif I2C_MODE == I2C_MODULE
#include "i2c_module2.h"
#endif


#define WRITE_CB(x)     x & 0xFE
#define READ_CB(x)      x | 0x01

static inline __attribute__((always_inline)) void disableInt() {
#ifndef FREE_INT
        IEC0bits.T1IE = 0;
        IEC0bits.T2IE = 0;
#endif
}

static inline __attribute__((always_inline)) void enableInt() {
#ifndef FREE_INT
        IEC0bits.T1IE = 1;
        IEC0bits.T2IE = 1;
#endif
}


void write_protect_enable();
void write_protect_disable();

    
    
void Init_I2C();