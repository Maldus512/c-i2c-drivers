#include <string.h>
#include "24LC16.h"

// unsigned char byteWrite_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char
// data)
//{
// ControlByte &= 0xF1;
// ControlByte |= HighAdd << 1;
// int counter = 0;
//
// if (HighAdd > HIGH_ADD_LIMIT)
//{
// return 1;
//}
//
// disableInt();
// write_protect_disable();
//
// idle(); //Ensure Module is Idle
// startCondition(); //Generate Start COndition
// masterWrite(ControlByte); //Write Control byte
// idle();
//
// do
//{
// counter++;
// if (readAck())
//{
// restartCondition();
// masterWrite(ControlByte); //Write Control byte
// idle();
// if (readAck())
//{
// continue;
//}
//}
// masterWrite(LowAdd); //Write Low Address
// idle();
//
// if (readAck())
//{
// continue;
//}
// masterWrite(data); //Write Data
// idle();
//}
// while (readAck() && counter <= 10);
//
// if (counter > 10) {
// enableInt();
// write_protect_enable();
// return -1;
//}
//
// stopCondition(); //Initiate Stop Condition
// EEAckPolling(ControlByte); //perform Ack Polling
//
// enableInt();
// write_protect_enable();
//
// return (0);
//}
//
//
//
///*----------------------------------------------------------------------------*/
///*  byteRead_24XX16                                                           */
///*----------------------------------------------------------------------------*/
// unsigned char byteRead_24XX16(unsigned char ControlByte, unsigned char HighAdd, unsigned char LowAdd, unsigned char
// *Data)
//{
// int counter = 0;
// if (HighAdd > HIGH_ADD_LIMIT)
//{
// return 1;
//}
// ControlByte = WRITE_CB(ControlByte);
// ControlByte &= 0xF1;
// ControlByte |= HighAdd << 1;
//
//
// disableInt();
// idle(); //Wait for bus Idle
// startCondition(); //Generate Start condition
// masterWrite(ControlByte); //send control byte for write
// idle(); //Wait for bus Idle
//
// do
//{
// counter++;
// if (readAck())
//{
// restartCondition();
// masterWrite(ControlByte);
// idle();
// if (readAck())
//{
// continue;
//}
//}
// masterWrite(LowAdd); //Send Low Address
// idle(); //Wait for bus Idle
//}
// while (readAck() && counter <= 10);
//
//
// if (counter > 10) {
// enableInt();
// return -1;
//}
//
// ControlByte = READ_CB(ControlByte);
// restartCondition(); //Generate Restart
// masterWrite(ControlByte); //send control byte for Read
// idle(); //Wait for bus Idle
//
// if (readAck())
//{
// enableInt();
// return -1;
//}
//
///*--------------------------------------------------------------*/
///* Legge la ram del dispositivo                                 */
///*--------------------------------------------------------------*/
//
//*Data = masterRead();
// writeAck(1);
//
// stopCondition();
// enableInt();
// return (0);
//}
//
//
///*----------------------------------------------------------------------------*/
///*  pageWrite_24XX16                                                          */
///*----------------------------------------------------------------------------*/
// char pageWrite_24XX16(unsigned char ControlByte, unsigned char LowAdd, unsigned char *wrptr, int Length)
//{
// int i = 0;
// ControlByte = WRITE_CB(ControlByte);
// int counter = 0;
//
// disableInt();
// write_protect_disable();
//
// idle(); //Ensure Module is Idle
// startCondition(); //Generate Start COndition
// masterWrite(ControlByte); //Write Control byte
// idle();
//
// do
//{
// counter++;
// if (readAck())
//{
// restartCondition();
// masterWrite(ControlByte); //Write Control byte
// idle();
// if (readAck())
//{
// continue;
//}
//}
// masterWrite(LowAdd); //Write Low Address
// idle();
//}
// while (readAck() && counter <= 10);
//
// if (counter > 10) {
// enableInt();
// write_protect_enable();
// return -1;
//}
//
// for (i = 0; i < Length; i++)
//{
// masterWrite(*wrptr);
//
// if (readAck() != 0)
//{
//#ifdef WRITE_PROTECT
// WRITE_PROTECT = 1;
//#endif
// return -1;
//}
// wrptr ++;
//}
//
//
// stopCondition(); //Generate a stop
// EEAckPolling(ControlByte); //perform Ack Polling
//
// enableInt();
// write_protect_enable();
// return 0;
//}
//
//
//
///*----------------------------------------------------------------------------*/
///* I2C_Read_b                                                                 */
///*----------------------------------------------------------------------------*/
// int blockRead_24XX16 (unsigned char ControlByte, unsigned char cRegAddr, unsigned char* buffer, int nLen)
//{
// unsigned int counter = 0, i;
// ControlByte = WRITE_CB(ControlByte);
// disableInt();
// idle(); //Ensure Module is Idle
// startCondition(); //Initiate start condition
// masterWrite(ControlByte); //write 1 byte
// idle(); //Ensure module is Idle
//
// do
//{
// counter++;
// if (readAck())
//{
// restartCondition();
// masterWrite(ControlByte); //write 1 byte
// idle();
//
// if (readAck())
//{
// continue;
//}
//}
// masterWrite(cRegAddr); //Write Low word address
// idle(); //Ensure module is idle
//}
// while (counter <= 10 && readAck());
//
// if (counter > 10) {
// enableInt();
// return -1;
//}
//
// startCondition();
// ControlByte = READ_CB(ControlByte);
//
// masterWrite(ControlByte);
//
// if (readAck()) {
// enableInt();
// return -1;
//}
//
///*--------------------------------------------------------------*/
///* Legge la ram del dispositivo                                 */
///*--------------------------------------------------------------*/
//
// for(i = 0; i < nLen; i++)
//{
// if (i == nLen - 1) {
// buffer[i] = masterRead();
// writeAck(1);
//}
// else {
// buffer[i] = masterRead();
// writeAck(0);
//}
//}
//
// stopCondition();
// enableInt();
// return (0);
//}
//
//
//
//
//
//
//

// void ack_polling

int EE24LC16_sequential_write(i2c_driver_t driver, uint8_t block, uint8_t address, uint8_t *data, int len) {
    uint8_t buffer[PAGE_SIZE + 1];
    if (block > MAX_BLOCK)
        return -1;

    uint8_t controlbyte = driver.device_address | (block << 1);
    int     fulladdr    = (block << 8) | address;
    if (fulladdr + len >= MEM_SIZE)
        return -2;

    while (len > 0) {
        int pagesize = PAGE_SIZE - (address % PAGE_SIZE);
        pagesize     = (len < pagesize) ? len : pagesize;
        buffer[0]    = address;
        memcpy(&buffer[1], data, pagesize);

        int res = driver.i2c_transfer(controlbyte, buffer, pagesize + 1, NULL, 0);

        if (res)
            return res;

        len -= pagesize;
        data += pagesize;

        if (address + pagesize > 0xFF) {
            if (block == MAX_BLOCK) {
                return -3;
            } else {
                block++;
                address = 0;
            }
        } else {
            address += pagesize;
        }

        controlbyte &= 0xF1;
        controlbyte |= block << 1;
    }

    if (driver.ack_polling)
        driver.ack_polling(driver.device_address);

    return 0;
}

int EE24LC16_sequential_read(i2c_driver_t driver, uint8_t block, uint8_t address, uint8_t *data, int len) {
    if (block > MAX_BLOCK)
        return -1;

    uint8_t controlbyte = driver.device_address | (block << 1);
    int     fulladdr    = (block << 8) | address;
    if (fulladdr + len >= MEM_SIZE)
        return -2;

    return driver.i2c_transfer(controlbyte, &address, 1, data, len);
}