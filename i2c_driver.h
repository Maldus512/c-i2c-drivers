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

/* Funzioni che devono essere definite (come macro)
 * Per utilizzare l'implementazione di default di pageWrite
 * e blockRead
 * 
 * idle
 * startCondition
 * masterWrite
 * readAck
 * restartCondition
 * stopCondition
 * disableInt
 * enableInt
 * write_protect_disable
 * write_protect_enable
 * masterRead
 * writeAck
 * 
 */



void startCondition();
char masterWrite(unsigned char byte);
char readAck();
void restartCondition();
void stopCondition();
void disableInt();
void enableInt();
void write_protect_disable();
void write_protect_enable();
unsigned char masterRead();
void writeAck(char ack);
void idle();
    
    
void Init_I2C();