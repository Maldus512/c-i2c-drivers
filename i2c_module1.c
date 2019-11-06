#include "hardwareprofile.h"

#include "i2c_module2.h"
#include "EEPROM/24XX1025.h"
#include "EEPROM/24XX16.h"
#include <i2c.h>




void Init_I2C(unsigned int brg)
{
    I2C1STAT = 0x0000;
    I2C1RCV = 0x0000;
    I2C1TRN = 0x0000;
    IFS1bits.MI2C1IF = 0;
    IEC1bits.MI2C1IE = 0;
    CloseI2C1();
    OpenI2C1(I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
            I2C1_IPMI_DIS & I2C1_7BIT_ADD & I2C1_SLW_EN &
            I2C1_SM_DIS & I2C1_GCALL_DIS & I2C1_STR_DIS &
            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
            I2C1_STOP_DIS & I2C1_RESTART_DIS & I2C1_START_DIS, brg);
    
    IdleI2C1();
#ifdef WRITE_PROTECT_TRIS
    WRITE_PROTECT_TRIS = OUTPUT_PIN;
#endif
    write_protect_enable();
}

void startCondition()
{
    I2C1CONbits.SEN = 1; //StartI2C1();
    while (I2C1CONbits.SEN);
    delay_us(2);
}

void stopCondition()
{
    I2C1CONbits.PEN = 1; //StopI2C1();
    while (I2C1CONbits.PEN);
}

void restartCondition()
{
    I2C1CONbits.RSEN = 1; //RestartI2C1();
    while (I2C1CONbits.RSEN);
    //delay_us(2);
}

void idle()
{
    while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT);
}

void writeAck(char ack)
{
    I2C1CONbits.ACKDT = ack; //Set for Ack or not ack
    I2C1CONbits.ACKEN = 1;
    while (I2C1CONbits.ACKEN); //wait for ACK to complete
}


char readAck() {
    return I2C1STATbits.ACKSTAT;
}

void masterWrite(unsigned char byte) {
    MasterWriteI2C1(byte);
}

char masterRead() {
    return MasterReadI2C1();
}

int findAddress() {
    int found_counter = 0;
    int i = 0;
    unsigned char addr = 0;
    disableInt();
    
    for (i = 0; i <= 0xFF; i += 2) {
        startCondition();
        
        addr = (unsigned char) i;

        MasterWriteI2C1(addr);
        idle();

        if (!I2C1STATbits.ACKSTAT) {
            found_counter++;
            Nop();
            Nop();
            Nop();
        }
        stopCondition();
    }
    
    enableInt();
    return found_counter;
}

int I2C_write_register(unsigned char addr, unsigned char reg,const unsigned char *Data, int N)
{
    const unsigned char *ptr;
    unsigned char len;
    int counter  = 0;
    disableInt();
    startCondition();
    MasterWriteI2C1(addr);
    idle();
    
    do
    {
        counter++;
        ptr = Data;
        len = N;
        if (I2C1STATbits.ACKSTAT)
        {
            restartCondition();
            MasterWriteI2C1(addr);
            idle();
            
            if (I2C1STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C1(reg);
        idle();
        
        while (len--) {
            MasterWriteI2C1(*ptr++);
            idle();
            if (I2C1STATbits.ACKSTAT) {
                break;
            }
        }
    }
    while (I2C1STATbits.ACKSTAT && counter <= 10);
    
    if (counter > 10) {
        enableInt();
        return -1;
    }

    stopCondition();
    idle();
    enableInt();
    return 0;
}

int I2C_read_register(unsigned char addr, unsigned char reg, unsigned char *Data, int N)
{
    unsigned char *ptr;
    int counter = 0;
    
    ptr = Data;
    disableInt();
    startCondition();
    MasterWriteI2C1(addr);
    idle();
    
    do
    {
        counter++;
        if (I2C1STATbits.ACKSTAT)
        {
            restartCondition();
            MasterWriteI2C1(addr);
            idle();
            if (I2C1STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C1(reg);
        idle();
        
        if (I2C1STATbits.ACKSTAT)
        {
            continue;
        }
        restartCondition();
        MasterWriteI2C1(addr | 1);//repeat the device address with R/W = 1 (read operation)
        idle();
    }
    while (I2C1STATbits.ACKSTAT && counter <= 10);
    
    if (counter > 10) {
        enableInt();
        return -1;
    }
    
    MastergetsI2C1(N, ptr, 50000);
    stopCondition(); //Send the Stop condition
    enableInt();
    return 0;
}


int check_address(unsigned char control) {
    char ack;
    disableInt();
    idle(); //wait for bus Idle
    startCondition(); //Generate Start condition

    MasterWriteI2C1(control);
    ack = I2C1STATbits.ACKSTAT;
    stopCondition();
    enableInt();
    return ack == 0;
}