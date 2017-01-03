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

#include <p24FJ256GA106.h>

#include <string.h>
#include <libpic30.h>

#include "I2C_drv.h"

#define TRUE    1
#define FALSE   0







/*----------------------------------------------------------------------------*/
/*  Init_I2C_b                                                                */
/*----------------------------------------------------------------------------*/
void Init_I2C_b (void)
{
    CLK_I2C_B       = 1;    /* CLK I2C  */
    DATA_I2C_B      = 1;    /* DATI I2C */
    
    DD_CLK_I2C_B    = OUTPUT_PIN;   /* abilita la porta come uscita CK        */
    DD_DATA_I2C_B   = OUTPUT_PIN;   /* abilita la porta come uscita DATI      */
                                    /* il pin deve essere riprogrammato per   */
                                    /* leggere i dati in ingresso e ACK       */
                                    /* dall' I2C                              */
    
    D_WP_TRIS       = OUTPUT_PIN;
    D_WP = 0;
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
unsigned int EEAckPolling_b(unsigned char control)
{
    unsigned char ack;
    startCondition();

    I2CByteWrite(WRITE_CB(control));
    ack = readAck();
    while (ack)
    {
        stopCondition();
        startCondition(); //generate restart
        I2CByteWrite(control);
        ack = readAck();
    }
    stopCondition(); //send stop condition
    
    return (0);
}


/*----------------------------------------------------------------------------*/
/*  I2C_Write_b                                                               */
/*----------------------------------------------------------------------------*/
int I2C_Write_b (unsigned char cDevAddr, unsigned char cRegAddr, const unsigned char* pData, int nLen)
{
    unsigned int y;

    startCondition();
    I2CByteWrite(cDevAddr);
    
    do {
        if (readAck() != 0) {
            startCondition();
            I2CByteWrite(cDevAddr);
            if (readAck() != 0)
                continue;
        }
  
        I2CByteWrite(cRegAddr);
    } while(readAck() != 0);
    
    for (y = 0; y < nLen; y++)
    {
        I2CByteWrite(*pData);
        if (readAck() != 0)
            return -1;
        /*while(readAck() != 0)
            I2CByteWrite(pData);*/
        pData ++;
    }
    
    stopCondition();
    EEAckPolling_b(cDevAddr);
    return (0);
}



/*----------------------------------------------------------------------------*/
/* I2C_Read_b                                                                 */
/*----------------------------------------------------------------------------*/
int I2C_Read_b (unsigned char cDevAddr, unsigned char cRegAddr, unsigned char* buffer, int nLen)
{
    unsigned int y;   
    startCondition();
    cDevAddr = WRITE_CB(cDevAddr);
    I2CByteWrite(cDevAddr);
    
    do {
        if (readAck() != 0) {
            startCondition();
            I2CByteWrite(cDevAddr);
            if (readAck() != 0)
                continue;
        }
  
        I2CByteWrite(cRegAddr);
    } while(readAck() != 0);
    
    startCondition();
    cDevAddr = READ_CB(cDevAddr);
    
    I2CByteWrite(cDevAddr);
    if (readAck() != 0)
        return -1;
        
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    for(y = 0; y < nLen; y++)
    {
        if (y == nLen - 1)
            I2CByteRead(buffer, 1); 
        else 
            I2CByteRead(buffer, 0); 
        buffer++;
    }
    
    stopCondition();
    return (0);
}


