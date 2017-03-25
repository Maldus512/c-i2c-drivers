/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: 24XX1025.c                                                        */
/*                                                                            */
/*      gestione EEPROM 24XX1025                                              */
/*                                                                            */
/*  Autore: Mattia Maldini                                                    */
/*                                                                            */
/*  Data  : 20/01/2003      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 21/05/2016      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/


#include "HardwareProfile.h"
#include "MyI2C2.h"


unsigned int byteWrite_24XX1025(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char data) {
    disableInt();
    MyIdleI2C2(); //Ensure Module is Idle  
    
    MyIdleI2C2(); //Ensure Module is Idle
    MyStartI2C2(); //Generate Start COndition
    MasterWriteI2C2(ControlByte); //Write Control byte
    MyIdleI2C2();
    
    do
    {
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
        MasterWriteI2C2(HighAdd);
        MyIdleI2C2(); //Write High Address
        MasterWriteI2C2(LowAdd); //Write Low Address
        MyIdleI2C2();
        
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
        MasterWriteI2C2(data); //Write Data
        MyIdleI2C2();
    }
    while (I2C2STATbits.ACKSTAT);
    //ErrorCode = !I2C2STATbits.ACKSTAT;		//Return ACK Status

    MyStopI2C2(); //Initiate Stop Condition
    EEAckPolling(ControlByte); //perform Ack Polling
    
    enableInt();
    return (0);
}



unsigned int byteRead_24XX1025(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *Data) {
    disableInt();
    MyIdleI2C2(); //Wait for bus Idle
    MyStartI2C2(); //Generate Start condition
    MasterWriteI2C2(ControlByte); //send control byte for write
    MyIdleI2C2(); //Wait for bus Idle
    
    do
    {
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
        MasterWriteI2C2(HighAdd); //Send High Address
        MyIdleI2C2(); //Wait for bus Idle
        MasterWriteI2C2(LowAdd); //Send Low Address
        MyIdleI2C2(); //Wait for bus Idle
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
    }
    while (I2C2STATbits.ACKSTAT);

    MyRestartI2C2(); //Generate Restart
    MasterWriteI2C2(ControlByte | 0x01); //send control byte for Read
    MyIdleI2C2(); //Wait for bus Idle

    *Data = MasterReadI2C2();

    NotAckI2C2(); //send Not Ack
    MyStopI2C2(); //Send Stop Condition
    enableInt();
    return (0);
}


void pageWrite_24XX1025(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, int Length) {
    disableInt();
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
        MasterWriteI2C2(HighAdd); //send High Address
        MyIdleI2C2(); //wait for bus Idle
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
    
    enableInt(); // Enable ALL INTERRUPT
}


void pageWrite_24XX1025_eds(unsigned char ControlByte, unsigned char HighAdd,
        unsigned char LowAdd, __eds__ unsigned char *wrptr, int Length) {
    disableInt();
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
        MasterWriteI2C2(HighAdd); //send High Address
        MyIdleI2C2(); //wait for bus Idle
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
    
    enableInt(); // Enable ALL INTERRUPT
}



unsigned int sequentialWrite_24XX1025(unsigned char ControlByte, unsigned char HighAdd, 
        unsigned char LowAdd, unsigned char *wrptr, unsigned int Length)
{
    unsigned char page_size = 0;
    unsigned char device = 0;
    unsigned char block = 0;
    
    int fullAdd = (HighAdd << 8) | LowAdd;
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + Length >= BLOCK_SIZE && block == 1) {
        //Trying to write more bytes than the memory is actually capable of storing
        return 1;
    }
    
    while (Length > 0)
    {
        page_size = PAGE_SIZE - (LowAdd % PAGE_SIZE);
        page_size = (Length < page_size) ? Length : page_size;
        
        pageWrite_24XX1025(ControlByte, HighAdd, LowAdd, wrptr, page_size);
        
        Length -= page_size;
        wrptr += page_size;
        
        //Adjust addresses
        if (LowAdd + page_size  > 0xFF) {
            if (HighAdd == HIGH_ADD_LIMIT) {
                    block++;
                    HighAdd = 0x00;
                    LowAdd = 0x00;
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
        ControlByte |= block << 3;
        ControlByte |= device << 1;
        ControlByte |= 0x00; //R/W - write operation
    }
    return (0);
}

unsigned int sequentialWrite_24XX1025_eds(unsigned char ControlByte, unsigned char HighAdd, 
        unsigned char LowAdd,__eds__ unsigned char *wrptr, unsigned int Length)
{
    unsigned char page_size = 0;
    unsigned char device = 0;
    unsigned char block = 0;
    
    int fullAdd = (HighAdd << 8) | LowAdd;
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + Length >= BLOCK_SIZE && block == 1) {
        //Trying to write more bytes than the memory is actually capable of storing
        return 1;
    }
    
    while (Length > 0)
    {
        page_size = PAGE_SIZE - (LowAdd % PAGE_SIZE);
        page_size = (Length < page_size) ? Length : page_size;
        
        pageWrite_24XX1025_eds(ControlByte, HighAdd, LowAdd, wrptr, page_size);
        
        Length -= page_size;
        wrptr += page_size;
        
        //Adjust addresses
        if (LowAdd + page_size  > 0xFF) {
            if (HighAdd == HIGH_ADD_LIMIT) {
                    block++;
                    HighAdd = 0x00;
                    LowAdd = 0x00;
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
        ControlByte |= block << 3;
        ControlByte |= device << 1;
        ControlByte |= 0x00; //R/W - write operation
    }
    return (0);
}


void blockRead_24XX1025(unsigned char ControlByte, unsigned char HighAdd,
        unsigned char LowAdd, unsigned char *rdptr, unsigned int length) {
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
        MasterWriteI2C2(HighAdd); //Write High word address
        MyIdleI2C2(); //Ensure module is idle
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


unsigned int sequentialRead_24XX1025(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length)
{
    unsigned int block_size;
    unsigned char block, device;
    
    if (HighAdd > HIGH_ADD_LIMIT) {
        return 1;
    }
    
    int fullAdd = (HighAdd << 8) | LowAdd;
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + length >= BLOCK_SIZE && block == 1) {
        //Trying to read more bytes than the memory is actually capable of storing
        return 1;
    }
    
    while (length > 0) {
        if (fullAdd == 0x0000) {
            block_size = BLOCK_SIZE - 1;
        }
        else
        {
            block_size = BLOCK_SIZE - fullAdd;
        }
        block_size = (length < block_size) ? length : block_size;
        /*Read the data*/
        blockRead_24XX1025(ControlByte, HighAdd, LowAdd, rdptr, block_size);
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
        ControlByte |= block << 3;
        ControlByte |= device << 1;
        ControlByte |= 0x00; //R/W - write operation
    }
    return (0);
}

unsigned int sequentialRead_24XX1025_eds(unsigned char ControlByte, unsigned char HighAdd,
        unsigned char LowAdd, __eds__ unsigned char *rdptr, unsigned int length)
{
    unsigned int block_size;
    unsigned char block, device;
    unsigned char buffer[BUFFER_SIZE];
    
    if (HighAdd > HIGH_ADD_LIMIT) {
        return 1;
    }
    
    int fullAdd = (HighAdd << 8) | LowAdd;
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + length >= BLOCK_SIZE && block == 1) {
        //Trying to read more bytes than the memory is actually capable of storing
        return 1;
    }
    
    while (length > 0) {
        if (fullAdd == 0x0000) {
            block_size = BLOCK_SIZE - 1;
        }
        else
        {
            block_size = BLOCK_SIZE - fullAdd;
        }
        block_size = (length < block_size) ? length : block_size;
        block_size = (block_size < BUFFER_SIZE) ? block_size : BUFFER_SIZE;
        /*Read the data*/
        blockRead_24XX1025(ControlByte, HighAdd, LowAdd, buffer, block_size);
        memwrite_eds(rdptr, buffer, block_size);
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
        ControlByte |= block << 3;
        ControlByte |= device << 1;
        ControlByte |= 0x00; //R/W - write operation
    }
    return (0);
}