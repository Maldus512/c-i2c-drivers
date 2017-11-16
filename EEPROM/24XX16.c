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



#if I2C_MODE == I2C_MODULE && RAM_TYPE == MEM_24XX16

#include "MyI2C2.h"

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
    MyIdleI2C2(); //Ensure Module is Idle  
    
    MyIdleI2C2(); //Ensure Module is Idle
    MyStartI2C2(); //Generate Start COndition
    MasterWriteI2C2(ControlByte); //Write Control byte
    MyIdleI2C2();
    
    do
    {
        counter++;
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(ControlByte); //Write Control byte
            MyIdleI2C2();
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        //TODO: r u sure this is ok? Should the HighAdd be sent?
        //MasterWriteI2C2(HighAdd);
        //MyIdleI2C2(); //Write High Address
        MasterWriteI2C2(LowAdd); //Write Low Address
        MyIdleI2C2();
        
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
        MasterWriteI2C2(data); //Write Data
        MyIdleI2C2();
    }
    while (I2C2STATbits.ACKSTAT && counter <= 10);
    //ErrorCode = !I2C2STATbits.ACKSTAT;		//Return ACK Status
    
    if (counter > 10) {
        enableInt();
        return -1;
    }

    MyStopI2C2(); //Initiate Stop Condition
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
    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    
    disableInt();
    MyIdleI2C2(); //Wait for bus Idle
    MyStartI2C2(); //Generate Start condition
    MasterWriteI2C2(ControlByte); //send control byte for write
    MyIdleI2C2(); //Wait for bus Idle
    
    do
    {
        counter++;
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(ControlByte);
            MyIdleI2C2();
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(LowAdd); //Send Low Address
        MyIdleI2C2(); //Wait for bus Idle
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
    }
    while (I2C2STATbits.ACKSTAT && counter <= 10);
    
    if (counter > 10) {
        enableInt();
        return -1;
    }

    MyRestartI2C2(); //Generate Restart
    MasterWriteI2C2(ControlByte | 0x01); //send control byte for Read
    MyIdleI2C2(); //Wait for bus Idle

    *Data = MasterReadI2C2();

    NotAckI2C2(); //send Not Ack
    MyStopI2C2(); //Send Stop Condition
    enableInt();
    return (0);
}



/*----------------------------------------------------------------------------*/
/*  pageWrite_24XX16                                                          */
/*----------------------------------------------------------------------------*/
void pageWrite_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, int Length)
{
    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    disableInt();
    write_protect_disable();
    MyIdleI2C2(); //wait for bus Idle
    MyStartI2C2(); //Generate Start condition
    MasterWriteI2C2(ControlByte); //send controlbyte for a write
    MyIdleI2C2(); //wait for bus Idle
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(ControlByte); //send controlbyte for a write
            MyIdleI2C2();
            
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(LowAdd); //send Low Address
        MyIdleI2C2(); //wait for bus Idle
    }
    while (I2C2STATbits.ACKSTAT);

    while ( Length-- > 0)
    {
        MasterWriteI2C2(*wrptr++);
        while (I2C2STATbits.TBF);
        MyIdleI2C2();
    }
    MyIdleI2C2(); //wait for bus Idle
    MyStopI2C2(); //Generate a stop
    EEAckPolling(ControlByte); //perform Ack Polling
    
    enableInt();
    write_protect_enable();
}



/*----------------------------------------------------------------------------*/
/*  sequentialWrite_24XX16                                                    */
/*----------------------------------------------------------------------------*/
unsigned char sequentialWrite_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, unsigned int Length)
{
    unsigned char page_size = 0;
    unsigned char device = 0;
    unsigned char block = 0;
    
    if (HighAdd > HIGH_ADD_LIMIT) {
        return 1;
    }

    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    
    int fullAdd = (HighAdd << 8) | LowAdd;
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + Length >= MEM_SIZE) {
        return 1;
    }
    
    while (Length > 0)
    {
        page_size = PAGE_SIZE - (LowAdd % PAGE_SIZE);
        page_size = (Length < page_size) ? Length : page_size;
        
        pageWrite_24XX16(ControlByte, HighAdd, LowAdd, wrptr, page_size);
        
        Length -= page_size;
        wrptr += page_size;
        //Adjust addresses
        if (LowAdd + page_size  > 0xFF) {
            if (HighAdd == HIGH_ADD_LIMIT) {

                    return 1; //Overflow - should not happen here but in the previous control
                }
                else {
                    HighAdd++;
                    LowAdd = 0x00;
                }
        }
        else
        {
            LowAdd += page_size;
        }
        ControlByte &= 0xF0;
        ControlByte &= 0xF1;
        ControlByte |= HighAdd << 1;
        ControlByte |= 0x00; //R/W - write operation
    }
    return (0);
}



/*----------------------------------------------------------------------------*/
/*  blockRead_24XX16                                                          */
/*----------------------------------------------------------------------------*/
void blockRead_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length)
{
        
    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    disableInt();
    MyIdleI2C2(); //Ensure Module is Idle
    MyStartI2C2(); //Initiate start condition
    MasterWriteI2C2(ControlByte); //write 1 byte
    MyIdleI2C2(); //Ensure module is Idle
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(ControlByte); //write 1 byte
            MyIdleI2C2();
            
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(LowAdd); //Write Low word address
        MyIdleI2C2(); //Ensure module is idle
    }
    while (I2C2STATbits.ACKSTAT);
    
    MyRestartI2C2(); //Generate I2C Restart Condition
    MasterWriteI2C2(ControlByte | 0x01); //Write 1 byte - R/W bit should be 1 for read
    MyIdleI2C2(); //Ensure bus is idle
    MastergetsI2C2(length, rdptr, 50000);
    NotAckI2C2(); //Send Not Ack
    MyStopI2C2(); //Send stop condition
    
    enableInt();
}



/*----------------------------------------------------------------------------*/
/*  sequentialRead_24XX16                                                     */
/*----------------------------------------------------------------------------*/
unsigned char sequentialRead_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length)
{
    //TODO check for page/block/device overlapping
    unsigned int block_size;
    unsigned char block, device;
    
    if (HighAdd > HIGH_ADD_LIMIT) {
        return 1;
    }

    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
  
    int fullAdd = (HighAdd << 8) | LowAdd;
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + length >= MEM_SIZE) {
        return 1;
    }
    
    while (length > 0) {
        block_size = length;
        /*Read the data*/
        blockRead_24XX16(ControlByte, HighAdd, LowAdd, rdptr, block_size);
        
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





#elif I2C_MODE == I2C_BITBANG && RAM_TYPE == MEM_24XX16

#include "i2c_drv.h"

/*----------------------------------------------------------------------------*/
/*  byteWrite_24XX16                                                          */
/*----------------------------------------------------------------------------*/
unsigned char byteWrite_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char data)
{
    unsigned char ret;
    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    
    if (HighAdd > HIGH_ADD_LIMIT)
    {
        return 1;
    }
    
    ret = I2C_Write_b(ControlByte, LowAdd, &data, 1);
    
    return ret;
}



/*----------------------------------------------------------------------------*/
/*  byteRead_24XX16                                                           */
/*----------------------------------------------------------------------------*/
unsigned char byteRead_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *data)
{ 
    if (HighAdd > HIGH_ADD_LIMIT)
    {
        return 1;
    }
    
    ControlByte &= 0xF1;
    ControlByte |= HighAdd << 1;
    
    return I2C_Read_b(ControlByte, LowAdd, data, 1);
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
        
        
        res = I2C_Write_b(ControlByte, LowAdd, wrptr, page_size);
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
        res = I2C_Read_b (ControlByte, LowAdd, rdptr, block_size);
        
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

#endif
