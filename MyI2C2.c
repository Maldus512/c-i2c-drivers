#include "HardwareProfile.h"

#include "MyI2C2.h"



void Init_I2C()
{
    I2C2STAT = 0x0000;
    I2C2RCV = 0x0000;
    I2C2TRN = 0x0000;
    IFS3bits.MI2C2IF = 0;
    IEC3bits.MI2C2IE = 0;
    
    OpenI2C2(I2C2_ON & I2C2_IDLE_CON & I2C2_CLK_HLD &
            I2C2_IPMI_DIS & I2C2_7BIT_ADD & I2C2_SLW_EN &
            I2C2_SM_DIS & I2C2_GCALL_DIS & I2C2_STR_DIS &
            I2C2_NACK & I2C2_ACK_DIS & I2C2_RCV_DIS &
            I2C2_STOP_DIS & I2C2_RESTART_DIS & I2C2_START_DIS, (unsigned int) I2C_BRG);
    
    IdleI2C2();
}

void I2CWriteReg(unsigned char reg, unsigned char data, unsigned char addr)
{
    INTCON2bits.GIE = 0; // Disable ALL INTERRUPT
    
    MyStartI2C2();

    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(addr);
            MyIdleI2C2();
            
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(reg);
        MyIdleI2C2();
        
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
        MasterWriteI2C2(data);
        MyIdleI2C2();
        
    } while(I2C2STATbits.ACKSTAT);
    
    MyStopI2C2();
    
    INTCON2bits.GIE = 1; // Enable ALL INTERRUPT
}

unsigned char I2CReadReg(unsigned char reg, unsigned char addr)
{
    unsigned char valore = 0;

    INTCON2bits.GIE = 0; // Disable ALL INTERRUPT
    
    MyStartI2C2();

    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(addr);
            MyIdleI2C2();
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        
        MasterWriteI2C2(reg);
        MyIdleI2C2();
    }
    while (I2C2STATbits.ACKSTAT);

    MyRestartI2C2();
    MasterWriteI2C2(addr | 1);
    MyIdleI2C2();
    valore = MasterReadI2C2();

    MyStopI2C2();
    
    INTCON2bits.GIE = 1; // Enable ALL INTERRUPT

    return valore;
}

unsigned char
I2CWriteRegN(unsigned char addr, unsigned char reg, unsigned char *Data, unsigned char N)
{
    unsigned char *ptr;
    
    ptr = Data;
    
    INTCON2bits.GIE = 0; // Disable ALL INTERRUPT
    
    MyStartI2C2();
    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(addr);
            MyIdleI2C2();
            
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(reg);
        MyIdleI2C2();
    }
    while (I2C2STATbits.ACKSTAT);

    while (N--)
    {
        MasterWriteI2C2(*ptr++);
        MyIdleI2C2();
        MyAckI2C2();
    }
    MyStopI2C2();
    MyIdleI2C2();
    
    INTCON2bits.GIE = 1; // Enable ALL INTERRUPT
    
    return 1;
}

unsigned char I2CReadRegN(unsigned char addr, unsigned char reg, unsigned char *Data, unsigned char N)
{
    unsigned char *ptr;
    
    ptr = Data;
    
    INTCON2bits.GIE = 0; // Disable ALL INTERRUPT
    
    MyStartI2C2();
    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(addr);
            MyIdleI2C2();
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(reg);
        MyIdleI2C2();
        
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
        MyRestartI2C2();
        MasterWriteI2C2(addr | 1);//repeat the device address with R/W = 1 (read operation)
        MyIdleI2C2();
    }
    while (I2C2STATbits.ACKSTAT);
    
    MastergetsI2C2(N, ptr, 50000);
    MyStopI2C2(); //Send the Stop condition
    
    INTCON2bits.GIE = 1; // Enable ALL INTERRUPT
    
    return 1;
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
    MyIdleI2C2(); //wait for bus Idle
    MyStartI2C2(); //Generate Start condition
    
    if (I2C1STATbits.BCL)
    {
        return (-1); //Bus collision, return
    }
    else
    {
        if (MasterWriteI2C2(control))
        {
            Nop();
            Nop();
            Nop();
            //return (-3); //error return
        }
        MyIdleI2C2(); //wait for bus idle
        
        if (I2C1STATbits.BCL || I2C2STATbits.IWCOL)
        {
            return (-1); //error return
        }
        
//        while (!I2C2STATbits.ACKSTAT) {
        
        while (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2(); //generate restart
            if (I2C1STATbits.BCL)
            {
                return (-1); //error return
            }
            
            if (MasterWriteI2C2(control)) {
                Nop();
                Nop();
                Nop();
                //return (-3);
            }
            
            MyIdleI2C2();
        }
    }
    MyStopI2C2(); //send stop condition
    
    if (I2C1STATbits.BCL)
    {
        return (-1);
    }
    return (0);
}





/*********************************************************************
 * Function:        HDByteWriteI2C()
 *
 * Input:		ControlByte, HighAddr, LowAddr, Data.
 *
 * Output:		None.
 *
 * Overview:		perform a high density byte write of data byte, Data.
 *				starting at the address formed from HighAdd and LowAdd
 *
 * Note:			None
 ********************************************************************/
unsigned int HDByteWriteI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char data)
{
    INTCON2bits.GIE = 0; // Disable ALL INTERRUPT
    
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
    
    INTCON2bits.GIE = 1; // Enable ALL INTERRUPT
    
    return (0);
}





/*********************************************************************
 * Function:        HDByteReadI2C()
 *
 * Input:		Control Byte, HighAdd, LowAdd, *Data, Length.
 *
 * Output:		None.
 *
 * Overview:		Performs a low density read of Length bytes and stores in *Data array
 *				starting at Address formed from HighAdd and LowAdd.
 *
 * Note:			None
 ********************************************************************/
unsigned int HDByteReadI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *Data)
{
    INTCON2bits.GIE = 0; // Disable ALL INTERRUPT
    
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
    
    INTCON2bits.GIE = 1; // Enable ALL INTERRUPT
    
    return (0);
}





/*********************************************************************
 * Function:        HDPageWriteI2C()
 *
 * Input:		ControlByte, HighAdd, LowAdd, *wrptr.
 *
 * Output:		None.
 *
 * Overview:		Write a page of data from array pointed to be wrptr
 *				starting at address from HighAdd and LowAdd
 *
 * Note:			Address must start on a page boundary and the number
 *              of bytes to write must not overflow the page. No
 *              controls are carried on in thos function
 ********************************************************************/

void HDPageWriteI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, int Length)
{
    INTCON2bits.GIE = 0; // Disable ALL INTERRUPT
    
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
    
    INTCON2bits.GIE = 1; // Enable ALL INTERRUPT
}





/*********************************************************************
 * Function:        HDSequentialWriteI2C()
 *
 * Input:		ControlByte, HighAdd, LowAdd, *wrptr.
 *
 * Output:		Error code:
 *                  0 : everything fine
 *                  1 : too many bytes to write
 *
 * Overview:		Write a page of data from array pointed to be wrptr
 *				starting at address from HighAdd and LowAdd
 *
 * Note:			This function performs all necessary checks and
 *              elaborations on addresses and pages.
 ********************************************************************/
unsigned int HDSequentialWriteI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, unsigned int Length)
{
    unsigned char page_size = 0;
    unsigned char device = 0;
    unsigned char block = 0;
    
    int fullAdd = (HighAdd << 8) | LowAdd;
    
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + Length >= BLOCK_SIZE && block == 1)
    {
        //Trying to write more bytes than the memory is actually capable of storing
        return 1;
    }
    
    while (Length > 0)
    {
        page_size = PAGE_SIZE - (LowAdd % PAGE_SIZE);
        page_size = (Length < page_size) ? Length : page_size;
        
        HDPageWriteI2C(ControlByte, HighAdd, LowAdd, wrptr, page_size);
        
        Length -= page_size;
        wrptr += page_size;
        
        //Adjust addresses
        if (LowAdd >= 128)
        {
            if (HighAdd == 0xFF)
            {
                block++;
                HighAdd = 0x00;
                LowAdd = 0x00;
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
        ControlByte &= 0xF0;
        ControlByte |= block << 3;
        ControlByte |= device << 1;
        ControlByte |= 0x00; //R/W - write operation
    }
    return (0);
}





/*********************************************************************
 * Function:        HDBlockReadI2C()
 *
 * Input:		ControlByte, HighAdd, LowAdd, *rdptr, length.
 *
 * Output:		None.
 *
 * Overview:		Performs a sequential read of length bytes starting at address
 *				and places data in array pointed to by *rdptr
 *
 * Note:			No check on address and length are issued
 ********************************************************************/
void HDBlockReadI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length)
{
    INTCON2bits.GIE = 0; // Disable ALL INTERRUPT
    
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
    
    INTCON2bits.GIE = 1; // Enable ALL INTERRUPT
}





/*********************************************************************
 * Function:        HDSequentialReadI2C()
 *
 * Input:		ControlByte, HighAdd, LowAdd, *rdptr, length.
 *
 * Output:		None.
 *
 * Overview:		Performs a sequential read of length bytes starting at address
 *				and places data in array pointed to by *rdptr
 *
 * Note:			None
 ********************************************************************/
unsigned int HDSequentialReadI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length)
{
    //TODO check for page/block/device overlapping
    unsigned int block_size;
    unsigned char block, device;
    
    int fullAdd = (HighAdd << 8) | LowAdd;
    
    device = (ControlByte & 0x06) >> 1;
    block = (ControlByte & 0x08) >> 3;
    
    if (fullAdd + length >= BLOCK_SIZE && block == 1)
    {
        //attempting to read overflows
        return 1;
    }
    
    while (length > 0)
    {
        if (fullAdd == 0x0000)
        {
            block_size = BLOCK_SIZE - 1;
        }
        else
        {
            block_size = BLOCK_SIZE - fullAdd;
        }
        block_size = (length < block_size) ? length : block_size;
        /*Read the data*/
        HDBlockReadI2C(ControlByte, HighAdd, LowAdd, rdptr, block_size);
        
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
