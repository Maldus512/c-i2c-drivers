/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: 24XX16.c                                                          */
/*                                                                            */
/*      gestione EEPROM 24XX16                                                */
/*                                                                            */
/*  Autore: Mattia Maldini e Massimo Zanna                                    */
/*                                                                            */
/*  Data  : 02/01/2017      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 04/01/2017      REV  : 15.0                                       */
/*                                                                            */
/******************************************************************************/

#include "HardwareProfile.h"
#include "24XX16.h"
#include "i2c_driver.h"
#include "eeprom_common.h"


/*----------------------------------------------------------------------------*/
/*  byteWrite_24XX16                                                          */
/*----------------------------------------------------------------------------*/
unsigned char byteWrite_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char data)
{
    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    int counter = 0;
    
    if (HighAdd > HIGH_ADD_LIMIT)
    {
        return 1;
    }
    
    disableInt();
    write_protect_disable();
    
    idle(); //Ensure Module is Idle
    startCondition(); //Generate Start COndition
    masterWrite(ControlByte); //Write Control byte
    idle();
    
    do
    {
        counter++;
        if (readAck())
        {
            restartCondition();
            masterWrite(ControlByte); //Write Control byte
            idle();
            if (readAck())
            {
                continue;
            }
        }
        masterWrite(LowAdd); //Write Low Address
        idle();
        
        if (readAck())
        {
            continue;
        }
        masterWrite(data); //Write Data
        idle();
    }
    while (readAck() && counter <= 10);
    
    if (counter > 10) {
        enableInt();
        write_protect_enable();
        return -1;
    }

    stopCondition(); //Initiate Stop Condition
    EEAckPolling(ControlByte); //perform Ack Polling
    
    enableInt();
    write_protect_enable();
    
    return (0);
}



/*----------------------------------------------------------------------------*/
/*  byteRead_24XX16                                                           */
/*----------------------------------------------------------------------------*/
unsigned char byteRead_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *Data)
{ 
    int counter = 0;
    if (HighAdd > HIGH_ADD_LIMIT)
    {
        return 1;
    }
    ControlByte = WRITE_CB(ControlByte);
    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    
    
    disableInt();
    idle(); //Wait for bus Idle
    startCondition(); //Generate Start condition
    masterWrite(ControlByte); //send control byte for write
    idle(); //Wait for bus Idle
    
    do
    {
        counter++;
        if (readAck())
        {
            restartCondition();
            masterWrite(ControlByte);
            idle();
            if (readAck())
            {
                continue;
            }
        }
        masterWrite(LowAdd); //Send Low Address
        idle(); //Wait for bus Idle
    }
    while (readAck() && counter <= 10);
    
    
    if (counter > 10) {
        enableInt();
        return -1;
    }
    
    ControlByte = READ_CB(ControlByte);
    restartCondition(); //Generate Restart
    masterWrite(ControlByte); //send control byte for Read
    idle(); //Wait for bus Idle
    
    if (readAck())
    {
        enableInt();
        return -1;
    }
        
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    *Data = masterRead(); 
    writeAck(1);
    
    stopCondition();
    enableInt();
    return (0);
}


/*----------------------------------------------------------------------------*/
/*  pageWrite_24XX16                                                          */
/*----------------------------------------------------------------------------*/
char pageWrite_24XX16(unsigned char ControlByte, unsigned char LowAdd, unsigned char *wrptr, int Length)
{
    int i = 0;
    ControlByte = WRITE_CB(ControlByte);
    int counter = 0;
    
    disableInt();
    write_protect_disable();
    
    idle(); //Ensure Module is Idle
    startCondition(); //Generate Start COndition
    masterWrite(ControlByte); //Write Control byte
    idle();
    
    do
    {
        counter++;
        if (readAck())
        {
            restartCondition();
            masterWrite(ControlByte); //Write Control byte
            idle();
            if (readAck())
            {
                continue;
            }
        }
        masterWrite(LowAdd); //Write Low Address
        idle();
    }
    while (readAck() && counter <= 10);

    if (counter > 10) {
        enableInt();
        write_protect_enable();
        return -1;
    }
    
    for (i = 0; i < Length; i++)
    {
        masterWrite(*wrptr);
        
        if (readAck_bitbang() != 0)
        {
#ifdef WRITE_PROTECT
            WP_I2C_B = 1;
#endif
            return -1;
        }
        wrptr ++;
    }
    

    stopCondition(); //Generate a stop
    EEAckPolling(ControlByte); //perform Ack Polling
    
    enableInt();
    write_protect_enable();
    return 0;
}



/*----------------------------------------------------------------------------*/
/* I2C_Read_b                                                                 */
/*----------------------------------------------------------------------------*/
int blockRead_24XX16 (unsigned char ControlByte, unsigned char cRegAddr, unsigned char* buffer, int nLen)
{
    unsigned int counter = 0, i;   
    ControlByte = WRITE_CB(ControlByte);
    disableInt();
    idle(); //Ensure Module is Idle
    startCondition(); //Initiate start condition
    masterWrite(ControlByte); //write 1 byte
    idle(); //Ensure module is Idle
    
    do
    {
        counter++;
        if (readAck())
        {
            restartCondition();
            masterWrite(ControlByte); //write 1 byte
            idle();
            
            if (readAck())
            {
                continue;
            }
        }
        masterWrite(cRegAddr); //Write Low word address
        idle(); //Ensure module is idle
    }
    while (counter <= 10 && readAck());
    
    if (counter > 10) {
        enableInt();
        return -1;
    }
    
    startCondition();
    ControlByte = READ_CB(ControlByte);
    
    masterWrite(ControlByte);
    
    if (readAck()) {
        enableInt();
        return -1;
    }
        
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    for(i = 0; i < nLen; i++)
    {
        if (i == nLen - 1) {
            buffer[i] = masterRead();
            writeAck(1);
        }
        else {
            buffer[i] = masterRead();
            writeAck(0);
        }
    }
    
    stopCondition();
    enableInt();
    return (0);
}







/*----------------------------------------------------------------------------*/
/*  sequentialWrite_24XX16                                                    */
/*----------------------------------------------------------------------------*/
unsigned char sequentialWrite_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, unsigned int Length)
{
    unsigned char page_size = 0;
    unsigned char res = 0;
    
    if (HighAdd > HIGH_ADD_LIMIT)
    {
        return 1;
    }

    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    
    int fullAdd = (HighAdd << 8) | LowAdd;
    
    if (fullAdd + Length >= MEM_SIZE)
    {
        return 1;
    }
    
    while (Length > 0)
    {
        page_size = PAGE_SIZE - (LowAdd % PAGE_SIZE);
        page_size = (Length < page_size) ? Length : page_size;
        
        
        res = pageWrite_24XX16(ControlByte, LowAdd, wrptr, page_size);
        if (res != 0) {
            return res;
        }
        
        Length -= page_size;
        wrptr += page_size;
        
        //Adjust addresses
        if (LowAdd + page_size  > 0xFF)
        {
            if (HighAdd == HIGH_ADD_LIMIT)
            {
                    return 1; //Overflow - should not happen here but in the previous control
                }
                else
                {
                    HighAdd++;
                    LowAdd = 0x00;
                }
        }
        else
        {
            LowAdd += page_size;
        }
        ControlByte &= 0xF1;
        ControlByte |= HighAdd << 1;
    }
    return (0);
}



/*----------------------------------------------------------------------------*/
/*  sequentialRead_24XX16                                                     */
/*----------------------------------------------------------------------------*/
unsigned char sequentialRead_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length)
{
    //TODO check for page/block/device overlapping
    unsigned int block_size;
    unsigned char block, device, res;
    
    if (HighAdd > HIGH_ADD_LIMIT)
    {
        return 1;
    }
    
    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
  
    int fullAdd = (HighAdd << 8) | LowAdd;
    
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + length >= MEM_SIZE)
    {
        return 1;
    }
    
    while (length > 0)
    {
        block_size = length;
        /*Read the data*/
        res = blockRead_24XX16 (ControlByte, LowAdd, rdptr, block_size);
        
        if (res != 0)
            return res;
        
        /*Adjust addresses and pointers*/
        rdptr += block_size;
        length -= block_size;
        
        if (length == 0)
        {
            break;
        }
        
        block++;
        HighAdd = LowAdd = 0x00;
        fullAdd = 0x0000;
        
        ControlByte &= 0xF0;
        ControlByte &= 0xF1;
        ControlByte |= HighAdd << 1;
        ControlByte |= 0x00; //R/W - write operation
    }
    return (0);
}

