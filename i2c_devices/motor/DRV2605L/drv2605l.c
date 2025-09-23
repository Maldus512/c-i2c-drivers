#include "drv2605l.h"


#define REGISTER_ADDRESS_MODE             0x01
#define REGISTER_REAL_TIME_PLAYBACK_INPUT 0x02
#define REGISTER_ADDRESS_FEEDBACK_CONTROL 0x1A
#define REGISTER_ADDRESS_CONTROL3         0x1D


static int set_bit_in_register(i2c_driver_t driver, uint8_t address, uint8_t position, bool value);


int drv2605l_set_standby(i2c_driver_t driver, bool active) {
    return set_bit_in_register(driver, REGISTER_ADDRESS_MODE, 6, active);
}


int drv2605l_set_erm_open_loop(i2c_driver_t driver, bool open) {
    return set_bit_in_register(driver, REGISTER_ADDRESS_CONTROL3, 5, open);
}


int drv2605l_set_data_format_rtp(i2c_driver_t driver, enum drv2605l_data_format_rtp data_format_rtp) {
    return set_bit_in_register(driver, REGISTER_ADDRESS_CONTROL3, 3, data_format_rtp);
}


int drv2605l_set_lra_open_loop(i2c_driver_t driver, bool open) {
    return set_bit_in_register(driver, REGISTER_ADDRESS_CONTROL3, 0, open);
}


int drv2605l_set_rtp(i2c_driver_t driver, uint8_t rtp) {
    return i2c_write_register(driver, REGISTER_ADDRESS_MODE, &rtp, 1);
}


int drv2605l_set_mode(i2c_driver_t driver, enum drv2605l_mode mode) {
    const uint8_t mode_mask = 0x7;

    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, REGISTER_ADDRESS_MODE, &buffer, 1);
    if (res != 0) {
        return res;
    }

    buffer &= ~mode_mask;
    buffer |= (mode & mode_mask);

    return i2c_write_register(driver, REGISTER_ADDRESS_MODE, &buffer, 1);
}


int drv2605l_set_motor_type(i2c_driver_t driver, enum drv2605l_motor_type motor_type) {
    return set_bit_in_register(driver, REGISTER_ADDRESS_FEEDBACK_CONTROL, 7, motor_type);
}

#include <stdio.h>

static int set_bit_in_register(i2c_driver_t driver, uint8_t address, uint8_t position, bool value) {
    const uint8_t mask = 1 << position;

    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, address, &buffer, 1);
    if (res != 0) {
        printf("Unable to read register 0x%02i - 0x%02i\n", address, buffer);
        return res;
    }

    printf("0x%02i - 0x%02i\n", address, buffer);

    if (value) {
        buffer |= mask;
    } else {
        buffer &= ~mask;
    }

    return i2c_write_register(driver, address, &buffer, 1);
}
