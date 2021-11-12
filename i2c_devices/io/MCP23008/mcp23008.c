#include "i2c_common/i2c_common.h"
#include "mcp23008.h"

#define EXPANDER_DIR_ADD   0x00
#define EXPANDER_POL_ADD   0x01
#define EXPANDER_IOCON_ADD 0x05
#define EXPANDER_GPIO_ADD  0x09

#define SET_BIT(reg, index, val) (reg & (~(1 << index))) | ((val ? 1 : 0) << index)

static int update_single_register_bit(i2c_driver_t driver, uint8_t regaddr, int index, int value);

int mcp23008_set_gpio_polarity(i2c_driver_t driver, mcp23008_gpio_t gpio, int inverted) {
    return update_single_register_bit(driver, EXPANDER_POL_ADD, gpio, inverted > 0);
}

int mcp23008_set_gpio_direction(i2c_driver_t driver, mcp23008_gpio_t gpio, mcp23008_mode_t mode) {
    return update_single_register_bit(driver, EXPANDER_DIR_ADD, gpio, mode);
}

int mcp23008_toggle_gpio(i2c_driver_t driver, mcp23008_gpio_t gpio) {
    int level = 0;
    int res   = mcp23008_get_gpio_level(driver, gpio, &level);

    if (res) {
        return res;
    }
    return update_single_register_bit(driver, EXPANDER_GPIO_ADD, gpio, !level);
}

int mcp23008_set_gpio_direction_register(i2c_driver_t driver, uint8_t regvalue) {
    uint8_t command[2] = {EXPANDER_DIR_ADD, regvalue};
    uint8_t response[1] = {0};
    int res = driver.i2c_transfer(driver.device_address, command, 2, response ,1, driver.arg);
    return res;
}

int mcp23008_set_gpio_polarity_register(i2c_driver_t driver, uint8_t regvalue) {
     uint8_t command[2] = {EXPANDER_POL_ADD, regvalue};
    uint8_t response[1] = {0};
    int res = driver.i2c_transfer(driver.device_address, command, 2, response ,1, driver.arg);
    return res;
}

int mcp23008_set_gpio_register(i2c_driver_t driver, uint8_t regvalue) {
    
    uint8_t command[2] = {EXPANDER_GPIO_ADD, regvalue};
    uint8_t response[1] = {0};
    int res = driver.i2c_transfer(driver.device_address, command, 2, response ,1, driver.arg);
    return res;
}

int mcp23008_get_gpio_register(i2c_driver_t driver, uint8_t *regvalue) {
    uint8_t command[1] = {EXPANDER_GPIO_ADD};
    uint8_t response[1] = {0};
    int res = driver.i2c_transfer(driver.device_address, command, 1, response, 1, driver.arg);
    *regvalue = response[0];
    return res;
}

int mcp23008_get_gpio_level(i2c_driver_t driver, mcp23008_gpio_t gpio, int *level) {
    uint8_t current = 0;
    int     res     = mcp23008_get_gpio_register(driver, &current);
    *level = (current & (1 << gpio)) > 0;
    return res;
}

int mcp23008_set_gpio_level(i2c_driver_t driver, mcp23008_gpio_t gpio, int level) {
    return update_single_register_bit(driver, EXPANDER_GPIO_ADD, gpio, level);
}

static int update_single_register_bit(i2c_driver_t driver, uint8_t regaddr, int index, int value) {
    uint8_t command[2] = {regaddr,0};
    uint8_t response[1] = {0};
    int res = driver.i2c_transfer(driver.device_address, command, 1, response, 1, driver.arg);
    if (res) {
        return res;
    }
    uint8_t current = response[0];
    current         = SET_BIT(current, index, value);
    command[0]= regaddr;
    command[1] = current;
    res = driver.i2c_transfer(driver.device_address, command, 2, NULL, 0, driver.arg);
    return res;
}