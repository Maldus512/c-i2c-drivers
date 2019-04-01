/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: 24XX1025.h                                                        */
/*                                                                            */
/*      gestione EEPROM 24XX1025                                              */
/*                                                                            */
/*  Autore: Mattia Maldini                                                    */
/*                                                                            */
/*  Data  : 20/01/2003      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 21/05/2016      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#ifndef __24XX1025_H__
#define __24XX1025_H__

#include "i2c_driver.h"
#include "eeprom_common.h"

/* Queste due funzioni devono essere fornite dall'applicazione 
    Ne esistono delle versioni di default nella libreria */
int pageWrite_24XX1025(unsigned char ControlByte, unsigned char HighAdd, 
        unsigned char LowAdd, unsigned char *wrptr, int Length);

int pageWrite_24XX1025_eds(unsigned char ControlByte, unsigned char HighAdd, 
        unsigned char LowAdd,__eds__ unsigned char *wrptr, int Length);

int blockRead_24XX1025(unsigned char ControlByte, unsigned char HighAdd,
        unsigned char LowAdd, unsigned char *rdptr, int length);

unsigned int AbsSequentialReadI2C(unsigned char ControlByte, unsigned int add, 
         unsigned char *rdptr, unsigned int Length);
unsigned int AbsSequentialWriteI2C(unsigned char ControlByte, unsigned int add, 
         unsigned char *wrptr, unsigned int Length);

unsigned int AbsSequentialWriteI2C_eds(unsigned char ControlByte, unsigned int add, 
        __eds__ unsigned char *wrptr, unsigned int Length);

unsigned int AbsSequentialReadI2C_eds(unsigned char ControlByte, unsigned int add, 
        __eds__ unsigned char *rdptr, unsigned int Length);


unsigned int byteWrite_24XX1025(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd, unsigned char data);
unsigned int byteRead_24XX1025(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *Data);

unsigned int sequentialWrite_24XX1025(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, unsigned int Length);
unsigned int sequentialWrite_24XX1025_eds(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd,__eds__ unsigned char *wrptr, unsigned int Length);

unsigned int sequentialRead_24XX1025(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length);
unsigned int sequentialRead_24XX1025_eds(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd,__eds__ unsigned char *rdptr, unsigned int length);

#endif