#include "eeprom_common.h"
#include "HardwareProfile.h"

#define WRITE_CB(x)     x & 0xFE
#define READ_CB(x)      x | 0x01


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
    startCondition(); //Generate Start condition
    unsigned long counter  = 0;
     
    
    masterWrite(control);

    while (readAck())
    {
        restartCondition(); //generate restart
        masterWrite(control);
        __delay_us(100);
        if (counter++ > 100UL) {
            return -1;
        }
    }
    
    stopCondition(); //send stop condition
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
int blockRead_24XX16 (unsigned char ControlByte, unsigned char cRegAddr, unsigned char* buffer, int nLen)
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
        masterWrite(cRegAddr); //Write Low word address
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
/*  pageWrite_24XX16                                                          */
/*----------------------------------------------------------------------------*/
char pageWrite_24XX1025(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, int Length)
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
