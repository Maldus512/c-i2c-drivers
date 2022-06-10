#include <string.h>
#include "24AA32.h"


#define MAX_BLOCK          0xF
#define MEM_SIZE           0x1000     // 4 KiByte
#define BLOCK_SIZE         0x100      // 256 Byte
#define PAGE_SIZE          0x08       // 8 Byte
#define CACHE_SIZE         0x40       // 64 Byte
#define PAGE_WRITE_TIME_MS 5


int ee24aa32_sequential_write(i2c_driver_t driver, uint8_t block, uint8_t address, uint8_t *data, size_t len) {
    uint8_t buffer[CACHE_SIZE + 2];
    if (block > MAX_BLOCK) {
        return -1;
    }

    uint8_t controlbyte = driver.device_address;
    int     fulladdr    = (block << 8) | address;
    if (fulladdr + len >= MEM_SIZE) {
        return -2;
    }

    while (len > 0) {
        int pagesize = CACHE_SIZE - (address % CACHE_SIZE);
        pagesize     = (len < pagesize) ? len : pagesize;
        buffer[0]    = block;
        buffer[1]    = address;
        memcpy(&buffer[2], data, pagesize);

        int res = driver.i2c_transfer(controlbyte, buffer, pagesize + 2, NULL, 0, driver.arg);

        len -= pagesize;
        data += pagesize;

        if (res) {
            return res;
        } else if (driver.ack_polling != NULL && len > 0) {
            driver.ack_polling(driver.device_address, driver.arg);
        } else if (driver.delay_ms != NULL && len > 0) {
            driver.delay_ms(PAGE_WRITE_TIME_MS * (pagesize / PAGE_SIZE));
        }

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

int ee24aa32_sequential_read(i2c_driver_t driver, uint8_t block, uint8_t address, uint8_t *data, size_t len) {
    if (block > MAX_BLOCK) {
        return -1;
    }

    uint8_t controlbyte = driver.device_address;
    int     fulladdr    = (block << 8) | address;
    if (fulladdr + len >= MEM_SIZE) {
        return -2;
    }

    uint8_t buffer[2] = {block, address};

    return driver.i2c_transfer(controlbyte, buffer, sizeof(buffer), data, len, driver.arg);
}


int ee24aa32_wait_for_cache_flush(i2c_driver_t driver, size_t bytes_written) {
    if (driver.ack_polling) {
        driver.ack_polling(driver.device_address, driver.arg);
        return 0;
    } else if (driver.delay_ms) {
        if ((bytes_written % PAGE_SIZE) != 0) {
            bytes_written += (PAGE_SIZE - (bytes_written % PAGE_SIZE));
        }
        bytes_written = bytes_written > CACHE_SIZE ? CACHE_SIZE : bytes_written;
        driver.delay_ms((bytes_written / PAGE_SIZE) * PAGE_WRITE_TIME_MS);
        return 0;
    } else {
        return -1;
    }
}