#include <stddef.h>
#include "../../../i2c_common/i2c_common.h"
#include "MCP9800.h"

#define RESOLUTION_BIT_SHIFT 5

double MCP9800_read_temperature(i2c_driver_t driver) {
    uint8_t cmd = MCP9800_TEMPERATURE_REGISTER;
    uint8_t buffer[2];
    int     res;

    if ((res = driver.i2c_transfer(driver.device_address, &cmd, 1, buffer, 2, driver.arg)))
        return 0;

    int negative = (buffer[0] & 0x80) > 0;

    double temperature = (double)(buffer[0] & 0x7F);
    temperature += (double)(buffer[1] >> 4) / 16;
    temperature *= negative ? -1 : 1;

    return temperature;
}

int MCP9800_set_resolution(i2c_driver_t driver, mcp9800_resolution_t resolution) {
    int res;
    resolution &= 0x3;
    uint8_t cmd    = MCP9800_CONFIGURATION_REGISTER;
    uint8_t config = 0;

    if ((res = driver.i2c_transfer(driver.device_address, &cmd, 1, &config, 1, driver.arg)))
        return res;

    config &= ~(0x3 << RESOLUTION_BIT_SHIFT);
    config |= resolution << RESOLUTION_BIT_SHIFT;

    uint8_t buf[2] = {cmd, config};
    return driver.i2c_transfer(driver.device_address, buf, 2, NULL, 0, driver.arg);
}