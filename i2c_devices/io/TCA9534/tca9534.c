#include "../../../i2c_common/i2c_common.h"
#include "tca9534.h"


#define REGISTER_INPUT_PORT         0x00
#define REGISTER_OUTPUT_PORT        0x01
#define REGISTER_POLARITY_INVERSION 0x02
#define REGISTER_CONFIGURATION      0x03

#define CONFIGURATION_INPUT_PIN  1
#define CONFIGURATION_OUTPUT_PIN 0

#define POLARITY_INVERTED 1
#define POLARITY_NORMAL   0


int tca9534_configure_gpio_port(i2c_driver_t driver, uint8_t port) {
    return i2c_write_register(driver, REGISTER_CONFIGURATION, &port, 1);
}


int tca9534_configure_gpio_pin(i2c_driver_t driver, tca9534_pin_t pin, tca9534_pin_mode_t mode) {
    uint8_t data = 0;
    int     res  = i2c_read_register(driver, REGISTER_CONFIGURATION, &data, 1);
    if (res) {
        return res;
    }

    data = (data & (~(1 << pin))) |
           ((mode == TCA9534_PIN_MODE_OUTPUT ? CONFIGURATION_OUTPUT_PIN : CONFIGURATION_INPUT_PIN) << pin);

    return i2c_write_register(driver, REGISTER_CONFIGURATION, &data, 1);
}


int tca9534_read_input_port(i2c_driver_t driver, uint8_t *port) {
    return i2c_read_register(driver, REGISTER_INPUT_PORT, port, 1);
}


int tca9534_read_input_pin(i2c_driver_t driver, tca9534_pin_t pin, uint8_t *value) {
    uint8_t data = 0;
    int     res  = i2c_read_register(driver, REGISTER_INPUT_PORT, &data, 1);
    if (res) {
        return res;
    }

    *value = (data & (1 << pin)) > 0;
    return 0;
}


int tca9534_set_output_port(i2c_driver_t driver, uint8_t port) {
    return i2c_write_register(driver, REGISTER_OUTPUT_PORT, &port, 1);
}


int tca9534_set_output_pin(i2c_driver_t driver, tca9534_pin_t pin, uint8_t value) {
    uint8_t data = 0;
    int     res  = i2c_read_register(driver, REGISTER_OUTPUT_PORT, &data, 1);
    if (res) {
        return res;
    }

    data = (data & (~(1 << pin))) | ((value > 0) << pin);
    return i2c_write_register(driver, REGISTER_OUTPUT_PORT, &data, 1);
}