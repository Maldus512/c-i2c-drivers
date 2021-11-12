#ifndef MCP23S08_H_INCLUDED
#define MCP23S08_H_INCLUDED

#include "i2c_common/i2c_common.h"

typedef enum {
    MCP23008_OUTPUT_MODE = 0,
    MCP23008_INPUT_MODE  = 1,
} mcp23008_mode_t;

typedef enum {
    MCP23008_GPIO_1 = 0,
    MCP23008_GPIO_2,
    MCP23008_GPIO_3,
    MCP23008_GPIO_4,
    MCP23008_GPIO_5,
    MCP23008_GPIO_6,
    MCP23008_GPIO_7,
    MCP23008_GPIO_8,
} mcp23008_gpio_t;

#define MCP23008_DEFAULT_ADDR 0x40

int mcp23008_init(i2c_driver_t driver, uint8_t address, int addressable);
int mcp23008_set_gpio_polarity(i2c_driver_t driver, mcp23008_gpio_t gpio, int inverted);
int mcp23008_set_gpio_direction(i2c_driver_t driver, mcp23008_gpio_t gpio, mcp23008_mode_t mode);
int mcp23008_toggle_gpio(i2c_driver_t driver, mcp23008_gpio_t gpio);
int mcp23008_set_gpio_direction_register(i2c_driver_t driver, uint8_t regvalue);
int mcp23008_set_gpio_polarity_register(i2c_driver_t driver, uint8_t regvalue);
int mcp23008_set_gpio_register(i2c_driver_t driver, uint8_t regvalue);
int mcp23008_get_gpio_register(i2c_driver_t driver, uint8_t *regvalue);
int mcp23008_get_gpio_level(i2c_driver_t driver, mcp23008_gpio_t gpio, int *level);
int mcp23008_set_gpio_level(i2c_driver_t driver, mcp23008_gpio_t gpio, int level);

#endif