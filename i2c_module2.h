/* 
 * File:   MyI2C2.h
 * Author: Sabbiolino
 *
 * Created on 1 maggio 2016, 18.21
 */


#ifndef MYI2C2_H
#define	MYI2C2_H

#include "EEPROM/24XX1025.h"
#include "EEPROM/24XX16.h"
#include "HardwareProfile.h"
#include "utility.h"
#include "system.h"



void write_protect_disable();
void write_protect_enable();

void Init_I2C_module(unsigned int brg);

void I2CWriteReg(unsigned char reg, unsigned char data, unsigned char addr);
unsigned char I2CReadReg(unsigned char reg, unsigned char addr);
unsigned char I2CWriteRegN(unsigned char addr,unsigned char reg, unsigned char *Data, unsigned char N );
unsigned char I2CReadRegN(unsigned char addr,unsigned char reg, unsigned char *Data, unsigned char N );

unsigned int EEAckPolling(unsigned char control);
int findAddress();




#if I2C_MODE == I2C_MODULE
static inline __attribute__((always_inline)) void startCondition_m2()
{
    I2C2CONbits.SEN = 1; //StartI2C2();
    while (I2C2CONbits.SEN);
    delay_us(2);
}

static inline __attribute__((always_inline)) void stopCondition_m2()
{
    I2C2CONbits.PEN = 1; //StopI2C2();
    while (I2C2CONbits.PEN);
}

static inline __attribute__((always_inline)) void restartCondition_m2()
{
    I2C2CONbits.RSEN = 1; //RestartI2C2();
    while (I2C2CONbits.RSEN);
    //delay_us(2);
}

static inline __attribute__((always_inline)) void idle_m2()
{
    while (I2C2CONbits.SEN || I2C2CONbits.RSEN || I2C2CONbits.PEN || I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);
}

static inline __attribute__((always_inline)) void writeAck_m2()
{
    I2C2CONbits.ACKDT = 0; //Set for ACk
    I2C2CONbits.ACKEN = 1;
    while (I2C2CONbits.ACKEN); //wait for ACK to complete
}


static inline __attribute__((always_inline)) unsigned char readAck_m2() {
    return I2C2STATbits.ACKSTAT;
}
#endif


#endif	/* MYI2C2_H */

