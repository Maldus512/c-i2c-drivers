#include "../../../i2c_common/i2c_common.h"

#define HUMIDITY_INTEGER          2
#define HUMIDITY_DECIMAL_PLACE    3
#define TEMPERATURE_INTEGER       0
#define TEMPERATURE_DECIMAL_PLACE 1
#define CHECKSUM                  4


int zs05_read(i2c_driver_t driver, double *temperature, double *humidity) {
    uint8_t buffer[5] = {0};

    i2c_read_register(driver, HUMIDITY_INTEGER, &buffer[0], 1);
    i2c_read_register(driver, HUMIDITY_DECIMAL_PLACE, &buffer[1], 1);
    i2c_read_register(driver, TEMPERATURE_INTEGER, &buffer[2], 1);
    i2c_read_register(driver, TEMPERATURE_DECIMAL_PLACE, &buffer[3], 1);
    i2c_read_register(driver, CHECKSUM, &buffer[4], 1);

    if (temperature)
        *temperature = (double)buffer[2] + ((double)buffer[3]) * .2;
    if (humidity)
        *humidity = (double)buffer[0] + ((double)buffer[1]) * .2;

    return !(buffer[0] + buffer[1] + buffer[2] + buffer[3] == buffer[4]);
}