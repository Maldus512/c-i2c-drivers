#include "hardwareprofile.h"

#include "i2c_module2.h"
#include "EEPROM/24XX1025.h"
#include "EEPROM/24XX16.h"
#include <i2c.h>




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
#ifdef WRITE_PROTECT_TRIS
    WRITE_PROTECT_TRIS = OUTPUT_PIN;
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

void writeAck(char ack)
{
    I2C2CONbits.ACKDT = 1-ack; //Set for Ack or not ack
    I2C2CONbits.ACKEN = 1;
    while (I2C2CONbits.ACKEN); //wait for ACK to complete
}


char readAck() {
    return I2C2STATbits.ACKSTAT;
}

void masterWrite(unsigned char byte) {
    MasterWriteI2C2(byte);
}

char masterRead() {
    return MasterReadI2C2();
}

int findAddress() {
    int found_counter = 0;
    int i = 0;
    unsigned char addr = 0;
    disableInt();
    
    for (i = 0; i <= 0xFF; i += 2) {
        startCondition();
        
        addr = (unsigned char) i;

        MasterWriteI2C2(addr);
        idle();

        if (!I2C2STATbits.ACKSTAT) {
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
    MasterWriteI2C2(addr);
    idle();
    
    do
    {
        counter++;
        ptr = Data;
        len = N;
        if (I2C2STATbits.ACKSTAT)
        {
            restartCondition();
            MasterWriteI2C2(addr);
            idle();
            
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(reg);
        idle();
        
        while (len--) {
            MasterWriteI2C2(*ptr++);
            idle();
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
    MasterWriteI2C2(addr);
    idle();
    
    do
    {
        counter++;
        if (I2C2STATbits.ACKSTAT)
        {
            restartCondition();
            MasterWriteI2C2(addr);
            idle();
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(reg);
        idle();
        
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
        restartCondition();
        MasterWriteI2C2(addr | 1);//repeat the device address with R/W = 1 (read operation)
        idle();
    }
    while (I2C2STATbits.ACKSTAT && counter <= 10);
    
    if (counter > 10) {
        enableInt();
        return -1;
    }
    
    MastergetsI2C2(N, ptr, 50000);
    stopCondition(); //Send the Stop condition
    enableInt();
    return 0;
}


int check_address(unsigned char control) {
    char ack;
    disableInt();
    idle(); //wait for bus Idle
    startCondition(); //Generate Start condition

    MasterWriteI2C2(control);
    ack = I2C2STATbits.ACKSTAT;
    stopCondition();
    enableInt();
    return ack == 0;
}
