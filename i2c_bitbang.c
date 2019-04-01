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


#include "i2c_driver.h"

#define HIGH    1
#define LOW   0


/*----------------------------------------------------------------------------*/
/*  Init_I2C_b                                                                */
/*----------------------------------------------------------------------------*/
void Init_I2C (void)
{
    ANSBbits.ANSB2 = 0;
    I2C_CLK       = 1;    /* CLK I2C  */
    I2C_DATA_OUT      = 1;    /* DATI I2C */
    
    
    I2C_CLK_TRIS    = OUTPUT_PIN;   /* abilita la porta come uscita CK        */
    I2C_DATA_TRIS   = OUTPUT_PIN;   /* abilita la porta come uscita DATI      */
                                    /* il pin deve essere riprogrammato per   */
                                    /* leggere i dati in ingresso e ACK       */
                                    /* dall' I2C                              */
#ifdef WRITE_PROTECT
    WP_I2C_B        = 1;    /* WRITE PROTECT BIT */
    DD_WP_I2C_B     = OUTPUT_PIN;   /* se H sono in WRITE PROTECT, se L -> WR */
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



void CK_I2C (unsigned char ck)
{
    I2C_CLK = ck;
    __delay_us(10);
}

void startCondition() {
    I2C_DATA_TRIS = OUTPUT_PIN;
    I2C_DATA_OUT = HIGH;
    I2C_CLK = HIGH;
    I2C_DATA_OUT = LOW;
}

void restartCondition() {
    I2C_DATA_TRIS = OUTPUT_PIN;
    I2C_DATA_OUT = HIGH;
    I2C_CLK = HIGH;
    I2C_DATA_OUT = LOW;
}

void stopCondition() {
    I2C_DATA_TRIS = OUTPUT_PIN;//1;      /* dati in uscita dal micro             */
    I2C_DATA_OUT = 0;         /* stop condition                       */
    CK_I2C(1);
    I2C_DATA_OUT = 1;         /* stop condition                       */
    CK_I2C(0);
    CK_I2C(1);
}

void idle() {
    Nop();
}


char masterWrite(unsigned char byte) {
    int x = 0;
    I2C_DATA_TRIS = OUTPUT_PIN;
    for ( x = 0; x < 8; x++)
    {
        CK_I2C(0);
        I2C_DATA_OUT = (byte >> (7 - x)) & 0x01;
        __delay_us(1);
        CK_I2C(1);
    }
    return 0;
}


unsigned char masterRead() {
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

void writeAck(char ack) {
    I2C_DATA_TRIS = OUTPUT_PIN;//1;  /* dati in uscita dal micro             */
    I2C_DATA_OUT = ack;     /* invia ack                            */
    CK_I2C(1);
    CK_I2C(0);
}

char readAck() {
    unsigned char x;
    CK_I2C(0);
    I2C_DATA_OUT = 1;         //Set Nack
    I2C_DATA_TRIS = INPUT_PIN;
    CK_I2C(1);
    x = I2C_DATA_IN;//I2C_DATA_OUT;
    CK_I2C(0);
    __delay_us(1);
    return x;
}