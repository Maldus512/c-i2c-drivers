#ifndef MCP9800_H_INCLUDED
#define MCP9800_H_INCLUDED

#include "common/i2c_common.h"

#define MCP9800_TEMPERATURE_REGISTER   0
#define MCP9800_CONFIGURATION_REGISTER 1
#define MCP9800_HYSTERESIS_REGISTER    2
#define MCP9800_LIMIT_REGISTER         3

typedef enum {
    MCP9800_9BIT  = 0,
    MCP9800_10BIT = 1,
    MCP9800_11BIT = 2,
    MCP9800_12BIT = 3,
} mcp9800_resolution_t;

double MCP9800_read_temperature(i2c_driver_t driver);
int    MCP9800_set_resolution(i2c_driver_t driver, mcp9800_resolution_t resolution);

#endif