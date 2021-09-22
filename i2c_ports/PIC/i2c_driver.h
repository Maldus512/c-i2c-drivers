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

#include "hardwareprofile.h"


#define WRITE_CB(x)     (x & 0xFE)
#define READ_CB(x)      (x | 0x01)

void disableInt();
void enableInt();

void write_protect_enable();
void write_protect_disable();

int I2C_write_register(unsigned char, unsigned char, const unsigned char*, int);
int I2C_read_register (unsigned char, unsigned char, unsigned char*, int);
int I2C_read_current_register (unsigned char, unsigned char*, int);


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
void masterWrite(unsigned char byte);
char readAck();
void restartCondition();
void stopCondition();
void disableInt();
void enableInt();
void write_protect_disable();
void write_protect_enable();
char masterRead();
void writeAck(char ack);
void idle();

    
    
void i2c_bitbang_init(unsigned int delay);