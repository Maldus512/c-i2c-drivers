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

#if I2C_MODE == I2C_BITBANG
#include "i2c_bitbang.h"
#elif I2C_MODE == I2C_MODULE
#include "i2c_module2.h"
#endif


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



void startCondition()
{
#if I2C_MODE == I2C_BITBANG
    startCondition_bitbang();
#elif I2C_MODE == I2C_MODULE
    startCondition_m2();
#endif
}

void stopCondition()
{
#if I2C_MODE == I2C_BITBANG
    stopCondition_bitbang();
#elif I2C_MODE == I2C_MODULE
    stopCondition_m2();
#endif
}

void restartCondition()
{
#if I2C_MODE == I2C_BITBANG
    restartCondition_bitbang();
#elif I2C_MODE == I2C_MODULE
    restartCondition_m2();
#endif
}

void idle()
{
#if I2C_MODE == I2C_BITBANG
    idle_bitbang();
#elif I2C_MODE == I2C_MODULE
    idle_m2();
#endif
}

/* Nota: la lettura di un ack via bitbang prevede anche un colpo
    di clock; come conseguenza tutte le scritture devono essere seguite
    da un controllo per ack */
char readAck()
{
#if I2C_MODE == I2C_BITBANG
    return readAck_bitbang();
#elif I2C_MODE == I2C_MODULE
    return readAck_m2();
#endif
}

void masterWrite(unsigned char byte)
{
#if I2C_MODE == I2C_BITBANG
    masterWrite_bitbang(byte);
#elif I2C_MODE == I2C_MODULE
    MasterWriteI2C2(byte);
#endif
}


unsigned char masterRead()
{
#if I2C_MODE == I2C_BITBANG
    return masterRead_bitbang();
#elif I2C_MODE == I2C_MODULE
    return MasterReadI2C2();
#endif
}

    
void writeAck(char ack)
{
#if I2C_MODE == I2C_BITBANG
    writeAck_bitbang(ack);
#elif I2C_MODE == I2C_MODULE
    if (ack) {
        NotAckI2C2();
    }
    else {
        AckI2C2();
    }
#endif
}