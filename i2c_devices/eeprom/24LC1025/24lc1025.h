#ifndef EE24LC1025_H_INCLUDED
#define EE24LC1025_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"


int ee24lc1025_sequential_write(i2c_driver_t driver, uint8_t high_addr, uint8_t low_addr, uint8_t *data, size_t len);
int ee24lc1025_sequential_read(i2c_driver_t driver, uint8_t high_addr, uint8_t low_addr, uint8_t *data, size_t len);


#endif