#include <stdint.h>
#include <stdlib.h>
#include "24lc1025.h"


#define HIGH_ADD_LIMIT 0xFF
#define BLOCK_SIZE     0x10000     // 64 K Byte
#define PAGE_SIZE      0x80        // 128 Byte
#define MAX_BLOCK      0x7


int ee24lc1025_sequential_write(i2c_driver_t driver, uint8_t high_addr, uint8_t low_addr, uint8_t *data, size_t len) {
    uint8_t buffer[PAGE_SIZE + 2];
    uint8_t page_size    = 0;
    uint8_t device       = 0;
    uint8_t block        = 0;
    uint8_t control_byte = driver.device_address;

    uint16_t full_addr = (high_addr << 8) | low_addr;
    device             = (control_byte & 0x06) >> 1;
    block              = (control_byte & 0x08) >> 3;

    if (full_addr + len >= BLOCK_SIZE && block == 1) {
        // Trying to write more bytes than the memory is actually capable of storing
        return -1;
    }

    while (len > 0) {
        page_size = PAGE_SIZE - (low_addr % PAGE_SIZE);
        page_size = (len < page_size) ? len : page_size;

        buffer[0] = high_addr;
        buffer[1] = low_addr;
        memcpy(&buffer[2], data, page_size);

        int res = driver.i2c_transfer(control_byte, buffer, page_size + 2, NULL, 0, driver.arg);
        if (res) {
            return res;
        }

        len -= page_size;
        data += page_size;

        // Adjust addresses
        if (low_addr + page_size > 0xFF) {
            if (high_addr == HIGH_ADD_LIMIT) {
                if (block == MAX_BLOCK) {
                    device++;
                    block     = 0;
                    high_addr = 0;
                    low_addr  = 0;
                } else {
                    block++;
                    high_addr = 0x00;
                    low_addr  = 0x00;
                }
            } else {
                high_addr++;
                low_addr = 0x00;
            }
        } else {
            low_addr += page_size;
        }
        control_byte &= 0xF0;
        control_byte |= block << 3;
        control_byte |= device << 1;
        control_byte = I2C_WRITE_ADDR(control_byte);
    }
    return 0;
}


int ee24lc1025_sequential_read(i2c_driver_t driver, uint8_t high_addr, uint8_t low_addr, uint8_t *data, size_t len) {
    unsigned int block_size;
    uint8_t      block, device;
    uint8_t      address_buffer[2] = {0};

    uint8_t control_byte = driver.device_address;
    int     full_addr    = (high_addr << 8) | low_addr;
    device               = (control_byte & 0x06) >> 1;
    block                = (control_byte & 0x08) >> 3;

    if (full_addr + len >= BLOCK_SIZE && block == 1) {
        // Trying to read more bytes than the memory is actually capable of storing
        return 1;
    }

    while (len > 0) {
        if (full_addr == 0x0000) {
            block_size = BLOCK_SIZE - 1;
        } else {
            block_size = BLOCK_SIZE - full_addr;
        }
        block_size = (len < block_size) ? len : block_size;

        address_buffer[0] = high_addr;
        address_buffer[1] = low_addr;

        /*Read the data*/
        int res = driver.i2c_transfer(control_byte, address_buffer, 2, data, block_size, driver.arg);
        if (res) {
            return res;
        }
        /*Adjust addresses and pointers*/
        data += block_size;
        len -= block_size;

        if (len == 0) {
            break;
        }

        block++;
        high_addr = low_addr = 0x00;
        full_addr            = 0x0000;
        if (block > MAX_BLOCK) {
            device++;
            block = 0;
        }

        control_byte &= 0xF0;
        control_byte |= block << 3;
        control_byte |= device << 1;
        control_byte = I2C_WRITE_ADDR(control_byte);
    }
    return 0;
}