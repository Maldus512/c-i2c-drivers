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

#include <stdint.h>

void Init_I2C (void);

unsigned int pic_i2c_bitbang_port_ack_polling(unsigned char control);
int pic_i2c_bitbang_port_transfer(uint8_t devaddr, uint8_t *writebuf, int writelen, uint8_t *readbuf, int readlen);

int findAddress_b();
