#ifndef ZS05_H_INCLUDED
#define ZS05_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"

#define ZS05_DEFAULT_ADDRESS 0xB8

int zs05_read(i2c_driver_t driver, double *temperature, double *humidity);

#endif