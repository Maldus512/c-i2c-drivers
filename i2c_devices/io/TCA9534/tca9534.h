#ifndef TCA9534_H_INCLUDED
#define TCA9534_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define TCA9534_DEFAULT_ADDR 0x40


typedef enum {
    TCA9534_PIN_P0 = 0,
    TCA9534_PIN_P1,
    TCA9534_PIN_P2,
    TCA9534_PIN_P3,
    TCA9534_PIN_P4,
    TCA9534_PIN_P5,
    TCA9534_PIN_P6,
    TCA9534_PIN_P7,
} tca9534_pin_t;

typedef enum {
    TCA9534_PIN_MODE_OUTPUT = 0,
    TCA9534_PIN_MODE_INPUT,
} tca9534_pin_mode_t;

typedef enum {
    TCA9534_PIN_POLARITY_NORMAL = 0,
    TCA9534_PIN_POLARITY_INVERTED,
} tca9534_pin_polarity;


int tca9534_configure_gpio_port(i2c_driver_t driver, uint8_t port);
int tca9534_configure_gpio_pin(i2c_driver_t driver, tca9534_pin_t pin, tca9534_pin_mode_t mode);
int tca9534_read_input_port(i2c_driver_t driver, uint8_t *port);
int tca9534_set_output_port(i2c_driver_t driver, uint8_t port);
int tca9534_set_output_pin(i2c_driver_t driver, tca9534_pin_t pin, uint8_t value);

#endif