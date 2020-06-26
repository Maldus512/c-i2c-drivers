#include "i2c_common/i2c_common.h"

#define HUMIDITY_INTEGER          0
#define HUMIDITY_DECIMAL_PLACE    1
#define TEMPERATURE_INTEGER       2
#define TEMPERATURE_DECIMAL_PLACE 3
#define CHECKSUM                  4

#include <stdio.h>


int zs05_read(i2c_driver_t driver, double *temperature, double *humidity) {
    uint8_t buffer[5] = {0};
    int     res       = i2c_read_register(driver, HUMIDITY_INTEGER, buffer, 5);

    printf("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);

    return res;
}