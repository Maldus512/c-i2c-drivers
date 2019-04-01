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

#define HIGH                1
#define LOW                 0


void Init_I2C_bitbang (void);

int I2C_Write_b(unsigned char, unsigned char, const unsigned char*, int);

int I2C_Read_b (unsigned char, unsigned char, unsigned char*, int);

int I2C_CurrentRead_b (unsigned char cDevAddr, unsigned char* buffer, int nLen);

void CK_I2C (unsigned char ck);


int findAddress_b();
