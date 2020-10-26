#include <string.h>
#include "24LC16.h"

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
        else if (driver.ack_polling)
            driver.ack_polling(driver.device_address);

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