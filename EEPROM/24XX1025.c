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
#include "24XX1025.h"
#include "utility.h"




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
        
        if (pageWrite_24XX1025_eds(ControlByte, HighAdd, LowAdd, wrptr, page_size) != 0)
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