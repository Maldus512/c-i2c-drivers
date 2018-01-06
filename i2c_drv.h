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

#define WRITE_CB(x)     x & 0xFE
#define READ_CB(x)      x | 0x01


#define HIGH                1
#define LOW                 0



static inline __attribute__((always_inline)) void CK_I2C (unsigned char ck)
{
    CLK_I2C_B = ck;
    delay_us(10);
}

static inline __attribute__((always_inline)) void startCondition() {
    DD_DATA_I2C_B = OUTPUT_PIN;
    DATA_I2C_B = HIGH;
    CLK_I2C_B = HIGH;
    DATA_I2C_B = LOW;
}

static inline __attribute((always_inline)) void stopCondition() {
    DD_DATA_I2C_B = OUTPUT_PIN;//1;      /* dati in uscita dal micro             */
    DATA_I2C_B = 0;         /* stop condition                       */
    CK_I2C(1);
    DATA_I2C_B = 1;         /* stop condition                       */
    CK_I2C(0);
    CK_I2C(1);
}

static inline __attribute__((always_inline)) void I2CByteWrite(unsigned char byte) {
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

static inline __attribute__((always_inline)) void I2CByteRead(unsigned char* byte, unsigned char ack) {
    DD_DATA_I2C_B = INPUT_PIN;//0;  /* dati in ingesso                      */
    unsigned char x = 0;
    for (x = 0; x < 8; x++)
    {
        CK_I2C(1);
        *byte = (*byte << 1) | DATA_I2C_I;//DATA_I2C_B; /* dato in input */
        CK_I2C(0);
    }
    DD_DATA_I2C_B = OUTPUT_PIN;//1;  /* dati in uscita dal micro             */
    DATA_I2C_B = ack;     /* invia ack                            */
    CK_I2C(1);
    CK_I2C(0);
}

static inline __attribute__((always_inline)) unsigned char readAck() {
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

void Init_I2C (void);
void Init_I2C_b (void);

//int I2C_Write (unsigned char, unsigned char, const unsigned char*, int);
int I2C_Write_b(unsigned char, unsigned char, const unsigned char*, int);
//int I2C_Write_512 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen);
int I2C_Write_1024_b (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen);
//int I2C_Write_512_b2 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen);
void I2C_Write_1024_b2(unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen);

//int I2C_Read (unsigned char, unsigned char, unsigned char*, int);
int I2C_Read_b (unsigned char, unsigned char, unsigned char*, int);
//int I2C_Read_512 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen);
int I2C_Read_1024_b (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen);
//int I2C_Read_512_b2 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen);
void I2C_Read_1024_b2(unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen);

//void CK_I2C (unsigned char ck);
void CK_I2C (unsigned char ck);

//void Set_I2C_Add (unsigned char add, unsigned char slave_add);
void Set_I2C_Add_b (unsigned char add, unsigned char slave_add);
//void Set_I2C_Add_512 (unsigned char add_h, unsigned char add_l, unsigned char slave_add);
void Set_I2C_Add_512_b (unsigned char add_h, unsigned char add_l, unsigned char slave_add);

