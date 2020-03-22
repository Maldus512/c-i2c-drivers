#ifndef MCP9800_H_INCLUDED
#define MCP9800_H_INCLUDED

#include "common/i2c_common.h"

#define MCP9800_TEMPERATURE_REGISTER   0
#define MCP9800_CONFIGURATION_REGISTER 1
#define MCP9800_HYSTERESIS_REGISTER    2
#define MCP9800_LIMIT_REGISTER         3

double MCP9800_read_temperature(i2c_driver_t driver);

#endif