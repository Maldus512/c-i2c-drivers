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



static inline __attribute__((always_inline)) void CK_I2C (unsigned char ck)
{
    I2C_CLK = ck;
    delay_us(10);
}

static inline __attribute__((always_inline)) void startCondition_bitbang() {
    I2C_DATA_TRIS = OUTPUT_PIN;
    I2C_DATA_OUT = HIGH;
    I2C_CLK = HIGH;
    I2C_DATA_OUT = LOW;
}

static inline __attribute__((always_inline)) void restartCondition_bitbang() {
    I2C_DATA_TRIS = OUTPUT_PIN;
    I2C_DATA_OUT = HIGH;
    I2C_CLK = HIGH;
    I2C_DATA_OUT = LOW;
}

static inline __attribute__((always_inline)) void stopCondition_bitbang() {
    I2C_DATA_TRIS = OUTPUT_PIN;//1;      /* dati in uscita dal micro             */
    I2C_DATA_OUT = 0;         /* stop condition                       */
    CK_I2C(1);
    I2C_DATA_OUT = 1;         /* stop condition                       */
    CK_I2C(0);
    CK_I2C(1);
}

static inline __attribute__((always_inline)) void idle_bitbang() {
    Nop();
}


static inline __attribute__((always_inline)) void masterWrite_bitbang(unsigned char byte) {
    int x = 0;
    I2C_DATA_TRIS = OUTPUT_PIN;
    for ( x = 0; x < 8; x++)
    {
        CK_I2C(0);
        I2C_DATA_OUT = (byte >> (7 - x)) & 0x01;
        delay_us(1);
        CK_I2C(1);
    }
}


static inline __attribute__((always_inline)) char masterRead_bitbang() {
    I2C_DATA_TRIS = INPUT_PIN;//0;  /* dati in ingesso                      */
    int i = 0;
    unsigned char byte = 0;
    for (i = 0; i < 8; i++)
    {
        CK_I2C(1);
        byte = (byte << 1) | I2C_DATA_IN;//I2C_DATA_OUT; /* dato in input */
        CK_I2C(0);
    }
    
    return byte;
}

static inline __attribute__((always_inline)) void writeAck_bitbang(char ack) {
    I2C_DATA_TRIS = OUTPUT_PIN;//1;  /* dati in uscita dal micro             */
    I2C_DATA_OUT = ack;     /* invia ack                            */
    CK_I2C(1);
    CK_I2C(0);
}

static inline __attribute__((always_inline)) char readAck_bitbang() {
    unsigned char x;
    CK_I2C(0);
    I2C_DATA_OUT = 1;         //Set Nack
    I2C_DATA_TRIS = INPUT_PIN;
    CK_I2C(1);
    x = I2C_DATA_IN;//I2C_DATA_OUT;
    CK_I2C(0);
    delay_us(1);
    return x;
}

void Init_I2C_bitbang (void);

int I2C_Write_b(unsigned char, unsigned char, const unsigned char*, int);

int I2C_Read_b (unsigned char, unsigned char, unsigned char*, int);

int I2C_CurrentRead_b (unsigned char cDevAddr, unsigned char* buffer, int nLen);

void CK_I2C (unsigned char ck);


int findAddress_b();
