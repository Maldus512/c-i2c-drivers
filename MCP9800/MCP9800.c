#include "common/i2c_common.h"
#include "MCP9800.h"

double MCP9800_read_temperature(i2c_driver_t driver) {
    uint8_t cmd = MCP9800_TEMPERATURE_REGISTER;
    uint8_t buffer[2];
    int     res;

    if ((res = driver.i2c_transfer(driver.device_address, &cmd, 1, buffer, 2)))
        return res;

    int negative = (buffer[0] & 0x80) > 0;

    double temperature = (double)(buffer[0] & 0x7F);
    temperature += (double)(buffer[1] >> 4) / 16;
    temperature *= negative ? -1 : 1;

    return temperature;
}