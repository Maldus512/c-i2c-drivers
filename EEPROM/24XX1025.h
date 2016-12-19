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


unsigned int byteWrite_24XX1025(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd, unsigned char data);
unsigned int byteRead_24XX1025(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *Data);
void pageWrite_24XX1025(unsigned char ControlByte, unsigned char HighAdd, 
        unsigned char LowAdd, unsigned char *wrptr, int Length);
unsigned int sequentialWrite_24XX1025(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, unsigned int Length);
void blockRead_24XX1025(unsigned char ControlByte, unsigned char HighAdd,
        unsigned char LowAdd, unsigned char *rdptr, unsigned int length);
unsigned int sequentialRead_24XX1025(unsigned char ControlByte, 
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length);

#endif