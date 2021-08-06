#ifndef EE24LC16_H_INCLUDED
#define EE24LC16_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"

#define MAX_BLOCK  0x7
#define MEM_SIZE   0x800     // 2 KiByte
#define BLOCK_SIZE 0x100     // 256 Byte
#define PAGE_SIZE  0x10      // 16 Byte

#define EEPROM24LC16_DEFAULT_ADDRESS 0xA0

#define EE24LC16_SEQUENTIAL_WRITE(driver, address, data, len) EE24LC16_sequential_write(driver, ((address)>>8) & 0x7, ((address) & 0xff),data, len)
#define EE24CL16_SEQUENTIAL_READ(driver, address, data, len) EE24LC16_sequential_read(driver, ((address)>>8) & 0x7, ((address) & 0xff), data, len)

int EE24LC16_sequential_write(i2c_driver_t driver, uint8_t block, uint8_t address, uint8_t *data, int len);
int EE24LC16_sequential_read(i2c_driver_t driver, uint8_t block, uint8_t address, uint8_t *data, int len);

#endif
