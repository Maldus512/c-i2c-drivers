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

#define CLK_I2C_B       I2C_CLK
#define DATA_I2C_B      I2C_DATA_OUT
#define WP_I2C_B        WRITE_PROTECT

#define DD_CLK_I2C_B    I2C_CLK_TRIS
#define DD_DATA_I2C_B   I2C_DATA_TRIS
#define DD_WP_I2C_B     WRITE_PROTECT_TRIS
#define DATA_I2C_I      I2C_DATA_IN

#define EEPROM_0_ADDR     '\xD0'
#define EEPROM_1_ADDR     '\xD0'
#define EEPROM_2_ADDR     '\xD0'




#define HIGH                1
#define LOW                 0



static inline __attribute__((always_inline)) void CK_I2C (unsigned char ck)
{
    CLK_I2C_B = ck;
    delay_us(3);
}

static inline __attribute__((always_inline)) void startCondition_bitbang() {
    DD_DATA_I2C_B = OUTPUT_PIN;
    DATA_I2C_B = HIGH;
    CLK_I2C_B = HIGH;
    DATA_I2C_B = LOW;
}

static inline __attribute__((always_inline)) void restartCondition_bitbang() {
    DD_DATA_I2C_B = OUTPUT_PIN;
    DATA_I2C_B = HIGH;
    CLK_I2C_B = HIGH;
    DATA_I2C_B = LOW;
}

static inline __attribute__((always_inline)) void stopCondition_bitbang() {
    DD_DATA_I2C_B = OUTPUT_PIN;//1;      /* dati in uscita dal micro             */
    DATA_I2C_B = 0;         /* stop condition                       */
    CK_I2C(1);
    DATA_I2C_B = 1;         /* stop condition                       */
    CK_I2C(0);
    CK_I2C(1);
}

static inline __attribute__((always_inline)) void idle_bitbang() {
    Nop();
}


static inline __attribute__((always_inline)) void masterWrite_bitbang(unsigned char byte) {
    int x = 0;
    DD_DATA_I2C_B = OUTPUT_PIN;
    for ( x = 0; x < 8; x++)
    {
        CK_I2C(0);
        DATA_I2C_B = (byte >> (7 - x)) & 0x01;
        delay_us(1);
        CK_I2C(1);
    }
}


static inline __attribute__((always_inline)) char masterRead_bitbang() {
    DD_DATA_I2C_B = INPUT_PIN;//0;  /* dati in ingesso                      */
    int i = 0;
    unsigned char byte = 0;
    for (i = 0; i < 8; i++)
    {
        CK_I2C(1);
        byte = (byte << 1) | DATA_I2C_I;//DATA_I2C_B; /* dato in input */
        CK_I2C(0);
    }
    
    return byte;
}

static inline __attribute__((always_inline)) void writeAck_bitbang(char ack) {
    DD_DATA_I2C_B = OUTPUT_PIN;//1;  /* dati in uscita dal micro             */
    DATA_I2C_B = ack;     /* invia ack                            */
    CK_I2C(1);
    CK_I2C(0);
}

static inline __attribute__((always_inline)) char readAck_bitbang() {
    unsigned char x;
    CK_I2C(0);
    DATA_I2C_B = 1;         //Set Nack
    DD_DATA_I2C_B = INPUT_PIN;
    CK_I2C(1);
    x = DATA_I2C_I;//DATA_I2C_B;
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
