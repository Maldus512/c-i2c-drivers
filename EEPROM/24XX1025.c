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


#include "hardwareprofile.h"
#include "24XX1025.h"
#include "utility.h"


unsigned int AbsSequentialWriteI2C(unsigned char ControlByte, unsigned int add, 
         unsigned char *wrptr, unsigned int Length)
{
    return sequentialWrite_24XX1025(ControlByte, (unsigned char) add>>8, (unsigned char) (add&0xFF), wrptr, Length);
}

unsigned int AbsSequentialReadI2C(unsigned char ControlByte, unsigned int add, 
         unsigned char *rdptr, unsigned int Length)
{
    return sequentialRead_24XX1025(ControlByte, (unsigned char) add>>8, (unsigned char) (add&0xFF), rdptr, Length);
}

unsigned int AbsSequentialWriteI2C_eds(unsigned char ControlByte, unsigned int add, 
        __eds__ unsigned char *wrptr, unsigned int Length)
{
    return sequentialWrite_24XX1025_eds(ControlByte, (unsigned char) add>>8, (unsigned char) (add&0xFF), wrptr, Length);
}

unsigned int AbsSequentialReadI2C_eds(unsigned char ControlByte, unsigned int add, 
        __eds__ unsigned char *rdptr, unsigned int Length)
{
    return sequentialRead_24XX1025_eds(ControlByte, (unsigned char) add>>8, (unsigned char) (add&0xFF), rdptr, Length);
}

unsigned int byteWrite_24XX1025(unsigned char ControlByte, unsigned char HighAdd, 
        unsigned char LowAdd, unsigned char byte)
{
    return sequentialWrite_24XX1025(ControlByte, HighAdd, LowAdd, &byte, 1);
}


unsigned int byteRead_24XX1025(unsigned char ControlByte, unsigned char HighAdd, 
        unsigned char LowAdd, unsigned char *byte)
{
    return sequentialRead_24XX1025(ControlByte, HighAdd, LowAdd, byte, 1);
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
        
        if (pageWrite_24XX1025(ControlByte, HighAdd, LowAdd, wrptr, page_size) != 0)
            return 1;
        
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
    unsigned int read = 0;
    unsigned char page_size = 0;
    unsigned char device = 0;
    unsigned char block = 0;
    unsigned char buffer1[PAGE_SIZE];
    
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
        
        memread_eds(buffer1, &wrptr[read], page_size);
        if (pageWrite_24XX1025(ControlByte, HighAdd, LowAdd, buffer1, page_size) != 0)
            return 1;
        
        /*Adjust addresses and pointers*/
        read += page_size;        
        Length -= page_size;
        
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
        if (blockRead_24XX1025(ControlByte, HighAdd, LowAdd, rdptr, block_size) != 0)
            return 1;
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
        if (block > 3) {
            return 1;
        }
        
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
    unsigned int  written = 0;
    unsigned char block, device;
    unsigned char buffer1[BUFFER_SIZE];
    long block_size;
    
    if (HighAdd > HIGH_ADD_LIMIT) {
        return 1;
    }
    
    long fullAdd = (HighAdd << 8) | LowAdd;
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
        if (blockRead_24XX1025(ControlByte, HighAdd, LowAdd, buffer1, block_size) != 0)
            return 1;
        memwrite_eds(&rdptr[written], buffer1, block_size);
        /*Adjust addresses and pointers*/
        written += block_size;
        length -= block_size;
        
        if (length == 0) {
            break;
        }
        
        
        if (fullAdd + block_size < BLOCK_SIZE) {

            fullAdd += block_size;
            HighAdd = (fullAdd >> 8) & 0xFF;
            LowAdd = fullAdd & 0xFF;
        } else {
            block++;
            HighAdd = LowAdd = 0x00;
            fullAdd = 0x0000;
            if (block > 3) {
                return 1;
            }
        }

        ControlByte &= 0xF0;
        ControlByte |= block << 3;
        ControlByte |= device << 1;
        ControlByte |= 0x00; //R/W - write operation
    }
    return (0);
}




/*----------------------------------------------------------------------------*/
/*  pageWrite_24XX16                                                          */
/*----------------------------------------------------------------------------*/
int pageWrite_24XX1025(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, int Length)
{
    int i = 0;
    ControlByte = WRITE_CB(ControlByte);
    int counter = 0;
    
    disableInt();
    write_protect_disable();
    
    startCondition(); //Generate Start COndition
    masterWrite(ControlByte); //Write Control byte
    
    do
    {
        counter++;
        if (readAck())
        {
            restartCondition();
            masterWrite(ControlByte); //Write Control byte
            if (readAck())
            {
                continue;
            }
        }
        masterWrite(HighAdd); //Write Low Address
        if (readAck())
        {
            restartCondition();
            continue;
        }
        masterWrite(LowAdd); //Write Low Address
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
        if (readAck() != 0)
        {
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
int blockRead_24XX1025 (unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char* buffer, int nLen)
{
    unsigned int counter = 0, i;   
    ControlByte = WRITE_CB(ControlByte);
    disableInt();
    startCondition(); //Initiate start condition
    masterWrite(ControlByte); //write 1 byte
    
    do
    {
        counter++;
        if (readAck())
        {
            restartCondition();
            masterWrite(ControlByte); //write 1 byte
            
            if (readAck())
            {
                continue;
            }
        }
        masterWrite(HighAdd); //Write Low word address
        if (readAck())
        {
            restartCondition();
            continue;
        }
        masterWrite(LowAdd);
    }
    while (counter <= 10 && readAck());
    
    if (counter > 10) {
        enableInt();
        return -1;
    }
    
    restartCondition();
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
