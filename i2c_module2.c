#include "HardwareProfile.h"

#include "i2c_module2.h"
#include "EEPROM/24XX1025.h"
#include "EEPROM/24XX16.h"







void Init_I2C(unsigned int brg)
{
    I2C2STAT = 0x0000;
    I2C2RCV = 0x0000;
    I2C2TRN = 0x0000;
    IFS3bits.MI2C2IF = 0;
    IEC3bits.MI2C2IE = 0;
    CloseI2C2();
    OpenI2C2(I2C2_ON & I2C2_IDLE_CON & I2C2_CLK_HLD &
            I2C2_IPMI_DIS & I2C2_7BIT_ADD & I2C2_SLW_EN &
            I2C2_SM_DIS & I2C2_GCALL_DIS & I2C2_STR_DIS &
            I2C2_NACK & I2C2_ACK_DIS & I2C2_RCV_DIS &
            I2C2_STOP_DIS & I2C2_RESTART_DIS & I2C2_START_DIS, brg);
    
    IdleI2C2();
#ifdef D_WP_I2C_TRIS
    D_WP_I2C_TRIS = OUTPUT_PIN;
#endif
    write_protect_enable();
}

void startCondition()
{
    I2C2CONbits.SEN = 1; //StartI2C2();
    while (I2C2CONbits.SEN);
    delay_us(2);
}

void stopCondition()
{
    I2C2CONbits.PEN = 1; //StopI2C2();
    while (I2C2CONbits.PEN);
}

void restartCondition()
{
    I2C2CONbits.RSEN = 1; //RestartI2C2();
    while (I2C2CONbits.RSEN);
    //delay_us(2);
}

void idle()
{
    while (I2C2CONbits.SEN || I2C2CONbits.RSEN || I2C2CONbits.PEN || I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);
}

void writeAck()
{
    I2C2CONbits.ACKDT = 0; //Set for ACk
    I2C2CONbits.ACKEN = 1;
    while (I2C2CONbits.ACKEN); //wait for ACK to complete
}


unsigned char readAck() {
    return I2C2STATbits.ACKSTAT;
}

int findAddress() {
    int found_counter = 0;
    int i = 0;
    unsigned char addr = 0;
    disableInt();
    
    for (i = 0; i <= 0xFF; i += 2) {
        MyStartI2C2();
        
        addr = (unsigned char) i;

        MasterWriteI2C2(addr);
        MyIdleI2C2();

        if (!I2C2STATbits.ACKSTAT) {
            found_counter++;
            Nop();
            Nop();
            Nop();
        }
        MyStopI2C2();
    }
    
    enableInt();
    return found_counter;
}

unsigned char
I2C_write_register(unsigned char addr, unsigned char reg, unsigned char *Data, unsigned char N)
{
    unsigned char *ptr;
    unsigned char len;
    int counter  = 0;
    disableInt();
    MyStartI2C2();
    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        counter++;
        ptr = Data;
        len = N;
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
        
        while (len--) {
            MasterWriteI2C2(*ptr++);
            MyIdleI2C2();
            if (I2C2STATbits.ACKSTAT) {
                break;
            }
        }
    }
    while (I2C2STATbits.ACKSTAT && counter <= 10);
    
    if (counter > 10) {
        enableInt();
        return -1;
    }

    MyStopI2C2();
    MyIdleI2C2();
    enableInt();
    return 0;
}

unsigned char I2C_read_register(unsigned char addr, unsigned char reg, unsigned char *Data, unsigned char N)
{
    unsigned char *ptr;
    int counter = 0;
    
    ptr = Data;
    disableInt();
    MyStartI2C2();
    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        counter++;
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
    while (I2C2STATbits.ACKSTAT && counter <= 10);
    
    if (counter > 10) {
        enableInt();
        return -1;
    }
    
    MastergetsI2C2(N, ptr, 50000);
    MyStopI2C2(); //Send the Stop condition
    enableInt();
    return 0;
}


int check_address(unsigned char control) {
    char ack;
    disableInt();
    MyIdleI2C2(); //wait for bus Idle
    MyStartI2C2(); //Generate Start condition

    MasterWriteI2C2(control);
    ack = I2C2STATbits.ACKSTAT;
    MyStopI2C2();
    enableInt();
    return ack == 0;
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
    
    unsigned long counter  = 0;
     
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
            
            if (counter++ > 100000UL) {
                return -1;
            }
        }
    }
    MyStopI2C2(); //send stop condition
    
    if (I2C1STATbits.BCL)
    {
        return (-1);
    }
    return (0);
}



