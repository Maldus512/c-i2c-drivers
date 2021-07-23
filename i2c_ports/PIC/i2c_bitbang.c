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

#include "hardwareprofile.h"

#include <string.h>

#include "i2c_bitbang.h"
#include "i2c_driver.h"

#include "../../i2c_common/i2c_common.h"



#define HIGH                1
#define LOW                 0
#define TRUE    1
#define FALSE   0
#define OUTPUT_PIN 0
#define INPUT_PIN 1


static int clock_delay = 10;



/*----------------------------------------------------------------------------*/
/*  Init_I2C_b                                                                */
/*----------------------------------------------------------------------------*/
void i2c_bitbang_init (void)
{
    I2C_CLK       = 1;    /* CLK I2C  */
    I2C_DATA_OUT      = 1;    /* DATI I2C */
    
    
    I2C_CLK_TRIS    = OUTPUT_PIN;   /* abilita la porta come uscita CK        */
    I2C_DATA_TRIS   = OUTPUT_PIN;   /* abilita la porta come uscita DATI      */
                                    /* il pin deve essere riprogrammato per   */
                                    /* leggere i dati in ingresso e ACK       */
                                    /* dall' I2C                              */
#ifdef WRITE_PROTECT
    WRITE_PROTECT        = 1;    /* WRITE PROTECT BIT */
    WRITE_PROTECT_TRIS     = OUTPUT_PIN;   /* se H sono in WRITE PROTECT, se L -> WR */
#endif
}


void CK_I2C (unsigned char ck)
{
    I2C_CLK = ck;
    __delay_us(clock_delay);
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


void masterWrite(unsigned char byte) {
    int x = 0;
    I2C_DATA_TRIS = OUTPUT_PIN;
    for ( x = 0; x < 8; x++)
    {
        CK_I2C(0);
        I2C_DATA_OUT = (byte >> (7 - x)) & 0x01;
        __delay_us(1);
        CK_I2C(1);
    }
}


char masterRead() {
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


/*********************************************************************
 * Function:        EEAckPolling()
 *
 * Input:		Control byte.
 *
 * Output:		error state.
 *
 * Overview:		polls the bus for an Acknowledge from device
 *
 * Note:			None
 ********************************************************************/
void pic_i2c_bitbang_port_ack_polling(uint8_t addr)
{
    unsigned char ack;
    startCondition();

    masterWrite(WRITE_CB(addr));
    ack = readAck();
    
    while (ack)
    {
        stopCondition();
        startCondition(); //generate restart
        masterWrite(addr);
        ack = readAck();
    }
    stopCondition(); //send stop condition
}



int findAddress_b() {
    int found_counter = 0;
    unsigned int i = 0;
    unsigned char i2caddr = 0;
//    disableInt();
    Nop();
    Nop();
    Nop();
    
    for (i = 0; i <= 0xFC; i += 2) {
        startCondition();
        
        i2caddr = (unsigned char) i;

        masterWrite(i);

        if (readAck() == 0) {
            found_counter++;
            Nop();
            Nop();
            Nop();
        } else {
            Nop();
            Nop();
            Nop();
        }
        stopCondition();
    }
    
//    enableInt();
    return found_counter;
}


int pic_i2c_bitbang_port_transfer(uint8_t devaddr, uint8_t *writebuf, int writelen, uint8_t *readbuf, int readlen) {
    
    unsigned int counter;
    if (writebuf!=NULL && writelen>0) {
         unsigned int y;
         write_protect_disable();
    counter = 0;
    
    
    
    do
    {
        counter++;
        startCondition();
        masterWrite(WRITE_CB(devaddr));
        if (readAck() != 0)
        {
            continue;
        }
        
        else break;
    }
    while(counter <= 10);
    
    if (counter > 10) {
        return -1;
    }
    
    for (y = 0; y < writelen; y++)
    {
        masterWrite(writebuf[y]);
        
        if (readAck() != 0)
        {
            write_protect_enable();
            return -1;
        }
  
    }
    
    stopCondition();
    //pic_i2c_bitbang_port_ack_polling(devaddr);
    
    write_protect_enable();
    }
    
    if (readbuf!=NULL && readlen>0) {
        
         unsigned int y;  
         counter=0;
        
        
    do {
        counter++;
        startCondition();
        masterWrite(READ_CB(devaddr));
        if (readAck() != 0) {
                continue;
        }
        
        else break;
    } while(counter<=10);
    
    if (counter>10)
        return -1;
    
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    for(y = 0; y < readlen; y++)
    {
        if (y == readlen - 1) {
            readbuf[y] = masterRead();
            writeAck(1);
        }
        else {
            readbuf[y] = masterRead(); 
            writeAck(0);
        }
    }
    
    stopCondition();
       
}
    return (0);
}
