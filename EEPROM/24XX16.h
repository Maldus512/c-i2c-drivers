/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: 24XX16.h                                                          */
/*                                                                            */
/*      gestione EEPROM 24XX16                                                */
/*                                                                            */
/*  Autore: Mattia Maldini e Massimo Zanna                                    */
/*                                                                            */
/*  Data  : 02/01/2017      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 04/01/2017      REV  : 15.0                                       */
/*                                                                            */
/******************************************************************************/

#ifndef __24XX16_H__
#define __24XX16_H__



unsigned char byteWrite_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char Data);
unsigned char byteRead_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *Data);

unsigned char sequentialWrite_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, unsigned int Length);
unsigned char sequentialRead_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length);

#endif
