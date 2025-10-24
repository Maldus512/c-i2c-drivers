#include "drv8235.h"


#define FAULT_STATUS_REGISTER_OFFSET 0x00
#define STATUS1_REGISTER_OFFSET      0x04
#define STATUS2_REGISTER_OFFSET      0x05
#define STATUS3_REGISTER_OFFSET      0x06
#define CONFIG0_REGISTER_OFFSET      0x09
#define CONFIG3_REGISTER_OFFSET      0x0C
#define CONFIG4_REGISTER_OFFSET      0x0D
#define CTRL0_REGISTER_OFFSET        0x0E
#define CTRL2_REGISTER_OFFSET        0x10


static int set_bit_in_register(i2c_driver_t driver, uint8_t address, uint8_t position, bool value);


int drv8235_get_faults(i2c_driver_t driver, struct drv8235_faults *faults) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, FAULT_STATUS_REGISTER_OFFSET, &buffer, 1);

    if (res) {
        return res;
    } else {
        faults->stall = buffer & (1 << 5);
        faults->ocp   = buffer & (1 << 4);
        faults->ovp   = buffer & (1 << 3);
        faults->tsd   = buffer & (1 << 2);
        faults->npor  = buffer & (1 << 1);
        return 0;
    }
}


int drv8235_read_status(i2c_driver_t driver, uint8_t *status1, uint8_t *status2, uint8_t *status3) {
    int res = i2c_read_register(driver, STATUS1_REGISTER_OFFSET, status1, 1);
    if (res) {
        return res;
    }

    res = i2c_read_register(driver, STATUS2_REGISTER_OFFSET, status2, 1);
    if (res) {
        return res;
    }

    res = i2c_read_register(driver, STATUS3_REGISTER_OFFSET, status3, 1);
    return res;
}


int drv8235_config_h_bridge_interface(i2c_driver_t driver, bool i2c) {
    return set_bit_in_register(driver, CONFIG4_REGISTER_OFFSET, 2, i2c);
}


int drv8235_config_internal_bridge_control(i2c_driver_t driver, bool en_in1, bool ph_in2) {
    int res = set_bit_in_register(driver, CONFIG4_REGISTER_OFFSET, 0, ph_in2);
    if (res) {
        return res;
    } else {
        return set_bit_in_register(driver, CONFIG4_REGISTER_OFFSET, 1, en_in1);
    }
}


int drv8235_config_pmode(i2c_driver_t driver, bool pwm) {
    return set_bit_in_register(driver, CONFIG4_REGISTER_OFFSET, 3, pwm);
}


int drv8235_config_manual_pwm(i2c_driver_t driver, bool manual_pwm) {
    return set_bit_in_register(driver, CONFIG0_REGISTER_OFFSET, 0, manual_pwm);
}


int drv8235_config_internal_vref(i2c_driver_t driver, bool internal) {
    return set_bit_in_register(driver, CONFIG3_REGISTER_OFFSET, 4, internal);
}


int drv8235_enable_output(i2c_driver_t driver, bool enable) {
    return set_bit_in_register(driver, CONFIG0_REGISTER_OFFSET, 7, enable);
}


int drv8235_clear_faults(i2c_driver_t driver) {
    return set_bit_in_register(driver, CONFIG0_REGISTER_OFFSET, 1, true);
}


int drv8235_set_pwm_frequency(i2c_driver_t driver, enum drv8235_pwm_frequency frequency) {
    return set_bit_in_register(driver, CTRL0_REGISTER_OFFSET, 2, frequency > 0);
}
#include "hal_data.h"
#include "bsp_pin_cfg.h"

int drv8235_set_pwm_duty(i2c_driver_t driver, uint8_t duty_percentage) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, CTRL2_REGISTER_OFFSET, &buffer, 1);
    if (res != 0) {
        return res;
    }

    uint8_t duty = (uint8_t)((((uint16_t)duty_percentage) * 63) / 100);

    buffer = (buffer & 0xC0);
    __NOP();
    __NOP();
    __NOP();
    buffer = buffer | duty;

    return i2c_write_register(driver, CTRL2_REGISTER_OFFSET, &buffer, 1);
}


static int set_bit_in_register(i2c_driver_t driver, uint8_t address, uint8_t position, bool value) {
    const uint8_t mask = (uint8_t)(1 << position);

    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, address, &buffer, 1);
    if (res != 0) {
        return res;
    }

    if (value) {
        buffer |= mask;
    } else {
        buffer &= ~mask;
    }

    return i2c_write_register(driver, address, &buffer, 1);
}
