/* 
 * File:   MyI2C2.h
 * Author: Sabbiolino
 *
 * Created on 1 maggio 2016, 18.21
 */


#ifndef MYI2C2_H
#define	MYI2C2_H

//#define MyStopI2C2()    I2C2CONbits.PEN = 1; while (I2C2CONbits.PEN);
//#define MyStartI2C2()   I2C2STATbits.ACKSTAT = 0; I2C2CONbits.SEN = 1; while (I2C2CONbits.SEN); delay_us(2);
//#define MyRestartI2C2() I2C2CONbits.RSEN = 1; while (I2C2CONbits.RSEN); //delay_us(2);
//#define MyIdleI2C2()    while(I2C2CONbits.SEN || I2C2CONbits.RSEN || I2C2CONbits.PEN || I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);

static inline __attribute__((always_inline)) void MyStartI2C2()
{
    I2C2CONbits.SEN = 1; //StartI2C2();
    while (I2C2CONbits.SEN);
    delay_us(2);
}

static inline __attribute__((always_inline)) void MyStopI2C2()
{
    I2C2CONbits.PEN = 1; //StopI2C2();
    while (I2C2CONbits.PEN);
}

static inline __attribute__((always_inline)) void MyRestartI2C2()
{
    I2C2CONbits.RSEN = 1; //RestartI2C2();
    while (I2C2CONbits.RSEN);
    //delay_us(2);
}

static inline __attribute__((always_inline)) void MyIdleI2C2()
{
    while (I2C2CONbits.SEN || I2C2CONbits.RSEN || I2C2CONbits.PEN || I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);
}

static inline __attribute__((always_inline)) void MyAckI2C2()
{
    I2C2CONbits.ACKDT = 0; //Set for ACk
    I2C2CONbits.ACKEN = 1;
    while (I2C2CONbits.ACKEN); //wait for ACK to complete
}

void Init_I2C();

void I2CWriteReg(unsigned char reg, unsigned char data, unsigned char addr);
unsigned char I2CReadReg(unsigned char reg, unsigned char addr);
unsigned char I2CWriteRegN(unsigned char addr,unsigned char reg, unsigned char *Data, unsigned char N );
unsigned char I2CReadRegN(unsigned char addr,unsigned char reg, unsigned char *Data, unsigned char N );

unsigned int HDByteWriteI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char data);
unsigned int HDByteReadI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *Data);
unsigned int HDSequentialWriteI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, unsigned int Length);
unsigned int HDSequentialReadI2C(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length);

//void I2C_Write_1024_B(unsigned char DevAddr, unsigned char reg1, unsigned char reg2, unsigned char* data);
//void I2C_Write_1024_S(unsigned char DevAddr, unsigned char reg1, unsigned char reg2, unsigned char* pData, unsigned int nLen);
//void I2C_Write_1024_SX(unsigned char DevAddr, unsigned char reg1, unsigned char reg2, unsigned char* pData, unsigned int nLen);
//
//
//unsigned char I2C_Read_1024_B(unsigned char DevAddr, unsigned char reg1, unsigned char reg2);
//void I2C_Read_1024_S(unsigned char DevAddr, unsigned char reg1, unsigned char reg2d, unsigned char* pBuffer, unsigned int nLen);
//void I2C_Read_1024_SX(unsigned char DevAddr, unsigned char reg1, unsigned char reg2d, unsigned char* pBuffer, unsigned int nLen);

#endif	/* MYI2C2_H */

