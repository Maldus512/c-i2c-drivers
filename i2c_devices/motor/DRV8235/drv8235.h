#ifndef DRV8235_H_INCLUDED
#define DRV8235_H_INCLUDED


#include <stdbool.h>
#include "../../../i2c_common/i2c_common.h"


#define DRV8235_DEFAULT_ADDRESS 0x60


struct drv8235_faults {
    bool stall;
    bool ocp;
    bool ovp;
    bool tsd;
    bool npor;
};


enum drv8235_pwm_frequency {
    DRV8235_PWM_FREQUENCY_50KHZ = 0,
    DRV8235_PWM_FREQUENCY_25KHZ,
};


int drv8235_get_faults(i2c_driver_t driver, struct drv8235_faults *faults);
int drv8235_clear_faults(i2c_driver_t driver);
int drv8235_config_h_bridge_interface(i2c_driver_t driver, bool i2c);
int drv8235_config_manual_pwm(i2c_driver_t driver, bool manual_pwm);
int drv8235_enable_output(i2c_driver_t driver, bool enable);
int drv8235_set_pwm_duty(i2c_driver_t driver, uint8_t duty_percentage);
int drv8235_set_pwm_frequency(i2c_driver_t driver, enum drv8235_pwm_frequency frequency);
int drv8235_config_pmode(i2c_driver_t driver, bool pwm);
int drv8235_config_internal_bridge_control(i2c_driver_t driver, bool en_in1, bool ph_in2);
int drv8235_read_status(i2c_driver_t driver, uint8_t *status1, uint8_t *status2, uint8_t *status3);
int drv8235_config_internal_vref(i2c_driver_t driver, bool internal);


#endif
