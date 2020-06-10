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
/*  U.mod.: 30/05/2020      REV  : 05.6                                       */
/*                                                                            */
/******************************************************************************/

#include "hardwareprofile.h"

#include <string.h>

#include "i2c_bitbang.h"
#include "i2c_driver.h"

#define TRUE    1
#define FALSE   0



#define CLOCK_DELAY 4 // il CLK da OSCILLOSCOPIO e' circa 100KHz (per la 1025 a 3,3V [400KHz a 5.0V])


/*----------------------------------------------------------------------------*/
/*  Init_I2C_b                                                                */
/*----------------------------------------------------------------------------*/
void Init_I2C (unsigned int brg)
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
    delay_us(CLOCK_DELAY);
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
    delay_us(1);
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
unsigned int EEAckPolling(unsigned char control)
{
    unsigned char ack;
    startCondition();

    masterWrite(WRITE_CB(control));
    ack = readAck();
    
    while (ack)
    {
        stopCondition();
        startCondition(); //generate restart
        masterWrite(control);
        ack = readAck();
    }
    stopCondition(); //send stop condition
    
    return (0);
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




int I2C_write_register (unsigned char cDevAddr, unsigned char cRegAddr, const unsigned char* pData, int nLen)
{
    unsigned int counter, y;
#ifdef WRITE_PROTECT
    WRITE_PROTECT = 0;
#endif
    counter = 0;
    startCondition();
    masterWrite(cDevAddr);
    
    do
    {
        counter++;
        if (readAck() != 0)
        {
            startCondition();
            masterWrite(cDevAddr);
            
            if (readAck() != 0)
                continue;
        }
        masterWrite(cRegAddr);
    }
    while(readAck() != 0 && counter <= 10);
    
    if (counter > 10) {
        return -1;
    }
    
    for (y = 0; y < nLen; y++)
    {
        masterWrite(*pData);
        
        if (readAck() != 0)
        {
#ifdef WRITE_PROTECT
            WRITE_PROTECT = 1;
#endif
            return -1;
        }
        pData ++;
    }
    
    stopCondition();
    EEAckPolling(cDevAddr);
    
#ifdef WRITE_PROTECT
    WRITE_PROTECT = 1;
#endif
    return (0);
}



int I2C_read_register (unsigned char cDevAddr, unsigned char cRegAddr, unsigned char* buffer, int nLen)
{
    unsigned int counter, y = 0; 
    char ack = 1;
    startCondition();
    cDevAddr = WRITE_CB(cDevAddr);
    masterWrite(cDevAddr);
    
    counter = 0;
    
    do {
        counter++;
        ack = readAck();
        if (ack != 0) {
            startCondition();
            masterWrite(cDevAddr);
            ack = readAck();
            if ( ack != 0)
                continue;
        }
  
        masterWrite(cRegAddr);
        ack = readAck();
    } while(ack != 0 && counter <= 10);
    
    if (counter > 10) {
        return -1;
    }
    
    startCondition();
    cDevAddr = READ_CB(cDevAddr);
    
    masterWrite(cDevAddr);
    if (readAck() != 0)
        return -1;
        
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    for(y = 0; y < nLen; y++)
    {
        if (y == nLen - 1) {
            *buffer = masterRead();
            writeAck(1);
        }
        else {
            *buffer = masterRead(); 
            writeAck(0);
        }
        buffer++;
    }
    
    stopCondition();
    return (0);
}


int I2C_read_current_register (unsigned char cDevAddr, unsigned char* buffer, int nLen)
{
    unsigned int y;   
    startCondition();
    cDevAddr = READ_CB(cDevAddr);
    masterWrite(cDevAddr);
        
    do {
        if (readAck() != 0) {
            startCondition();
            masterWrite(cDevAddr);
            if (readAck() != 0)
                continue;
        }
  
    } while(readAck() != 0);
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    for(y = 0; y < nLen; y++)
    {
        if (y == nLen - 1) {
            *buffer = masterRead();
            writeAck(1);
        }
        else {
            *buffer = masterRead(); 
            writeAck(0);
        }
        buffer++;
    }
    
    stopCondition();
    return (0);
}
