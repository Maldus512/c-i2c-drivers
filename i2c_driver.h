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
#include "system.h"
#include "i2c_bitbang.h"
#include "i2c_module2.h"


void write_protect_enable();
void write_protect_disable();

static inline __attribute__((always_inline)) void startCondition()
{
#if I2C_MODE == I2C_BITBANG
    startCondition_bitbang();
#elif I2C_MODE == I2C_MODULE
    startCondition_m2();
#endif
}

static inline __attribute__((always_inline)) void stopCondition()
{
#if I2C_MODE == I2C_BITBANG
    stopCondition_bitbang();
#elif I2C_MODE == I2C_MODULE
    startCondition_m2();
#endif
}

static inline __attribute__((always_inline)) void restartCondition()
{
#if I2C_MODE == I2C_BITBANG
    restartCondition_bitbang();
#elif I2C_MODE == I2C_MODULE
    restartCondition_m2();
#endif
}

static inline __attribute__((always_inline)) void idle()
{
#if I2C_MODE == I2C_BITBANG
    idle_bitbang();
#elif I2C_MODE == I2C_MODULE
    idle_m2();
#endif
}

static inline __attribute__((always_inline)) void writeAck()
{
#if I2C_MODE == I2C_BITBANG
    //writeAck_bitbang();
#elif I2C_MODE == I2C_MODULE
    writeAck_m2();
#endif
}


static inline __attribute__((always_inline)) char readAck()
{
#if I2C_MODE == I2C_BITBANG
    return readAck_bitbang();
#elif I2C_MODE == I2C_MODULE
    return readAck_m2();
#endif
}

static inline __attribute__((always_inline)) void masterWrite(unsigned char byte)
{
#if I2C_MODE == I2C_BITBANG
    masterWrite_bitbang(byte);
#elif I2C_MODE == I2C_MODULE
    MasterWriteI2C2(byte);
#endif
}
