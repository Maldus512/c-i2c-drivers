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

#define TRUE    1
#define FALSE   0







/*----------------------------------------------------------------------------*/
/*  Init_I2C_b                                                                */
/*----------------------------------------------------------------------------*/
void Init_I2C_bitbang (void)
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
    startCondition_bitbang();

    masterWrite_bitbang(WRITE_CB(control));
    ack = readAck_bitbang();
    
    while (ack)
    {
        stopCondition_bitbang();
        startCondition_bitbang(); //generate restart
        masterWrite_bitbang(control);
        ack = readAck_bitbang();
    }
    stopCondition_bitbang(); //send stop condition
    
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
        startCondition_bitbang();
        
        i2caddr = (unsigned char) i;

        masterWrite_bitbang(i);

        if (readAck_bitbang() == 0) {
            found_counter++;
            Nop();
            Nop();
            Nop();
        }
        stopCondition_bitbang();
    }
    
//    enableInt();
    return found_counter;
}




/*----------------------------------------------------------------------------*/
/*  I2C_Write_b                                                               */
/*----------------------------------------------------------------------------*/
int I2C_Write_b (unsigned char cDevAddr, unsigned char cRegAddr, const unsigned char* pData, int nLen)
{
    unsigned int counter, y;
#ifdef WRITE_PROTECT
    WRITE_PROTECT = 0;
#endif
    counter = 0;
    startCondition_bitbang();
    masterWrite_bitbang(cDevAddr);
    
    do
    {
        counter++;
        if (readAck_bitbang() != 0)
        {
            startCondition_bitbang();
            masterWrite_bitbang(cDevAddr);
            
            if (readAck_bitbang() != 0)
                continue;
        }
        masterWrite_bitbang(cRegAddr);
    }
    while(readAck_bitbang() != 0 && counter <= 10);
    
    if (counter > 10) {
        return -1;
    }
    
    for (y = 0; y < nLen; y++)
    {
        masterWrite_bitbang(*pData);
        
        if (readAck_bitbang() != 0)
        {
#ifdef WRITE_PROTECT
            WRITE_PROTECT = 1;
#endif
            return -1;
        }
        pData ++;
    }
    
    stopCondition_bitbang();
    EEAckPolling_b(cDevAddr);
    
#ifdef WRITE_PROTECT
    WRITE_PROTECT = 1;
#endif
    return (0);
}



/*----------------------------------------------------------------------------*/
/* I2C_Read_b                                                                 */
/*----------------------------------------------------------------------------*/
int I2C_Read_b (unsigned char cDevAddr, unsigned char cRegAddr, unsigned char* buffer, int nLen)
{
    unsigned int counter, y;   
    startCondition_bitbang();
    cDevAddr = WRITE_CB(cDevAddr);
    masterWrite_bitbang(cDevAddr);
    
    counter = 0;
    
    do {
        counter++;
        if (readAck_bitbang() != 0) {
            startCondition_bitbang();
            masterWrite_bitbang(cDevAddr);
            if (readAck_bitbang() != 0)
                continue;
        }
  
        masterWrite_bitbang(cRegAddr);
    } while(readAck_bitbang() != 0 && counter <= 10);
    
    if (counter > 10) {
        return -1;
    }
    
    startCondition_bitbang();
    cDevAddr = READ_CB(cDevAddr);
    
    masterWrite_bitbang(cDevAddr);
    if (readAck_bitbang() != 0)
        return -1;
        
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    for(y = 0; y < nLen; y++)
    {
        if (y == nLen - 1)
            masterRead_bitbang(buffer, 1); 
        else 
            masterRead_bitbang(buffer, 0); 
        buffer++;
    }
    
    stopCondition_bitbang();
    return (0);
}


int I2C_CurrentRead_b (unsigned char cDevAddr, unsigned char* buffer, int nLen)
{
    unsigned int y;   
    startCondition_bitbang();
    cDevAddr = READ_CB(cDevAddr);
    masterWrite_bitbang(cDevAddr);
        
    do {
        if (readAck_bitbang() != 0) {
            startCondition_bitbang();
            masterWrite_bitbang(cDevAddr);
            if (readAck_bitbang() != 0)
                continue;
        }
  
    } while(readAck_bitbang() != 0);
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    for(y = 0; y < nLen; y++)
    {
        if (y == nLen - 1)
            masterRead_bitbang(buffer, 1); 
        else 
            masterRead_bitbang(buffer, 0); 
        buffer++;
    }
    
    stopCondition_bitbang();
    return (0);
}