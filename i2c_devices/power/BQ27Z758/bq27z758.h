#ifndef BQ27Z758_H_INCLUDED
#define BQ27Z758_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define BQ27Z758_DEFAULT_ADDRESS 0xAA


enum bq27z758_security_mode {
    BQ27Z758_SECURITY_MODE_FULL_ACCESS = 0,
    BQ27Z758_SECURITY_MODE_RESERVED    = 1,
    BQ27Z758_SECURITY_MODE_UNSEALED    = 2,
    BQ27Z758_SECURITY_MODE_SEALED      = 3,

};


int bq27z758_get_control_status(i2c_driver_t driver, uint16_t *status);
int bq27z758_get_battery_status(i2c_driver_t driver, uint16_t *status);
int bq27z758_get_temperature(i2c_driver_t driver, int16_t *temperature_celsius);
int bq27z758_get_voltage(i2c_driver_t driver, uint16_t *voltage);
int bq27z758_get_current(i2c_driver_t driver, int16_t *current);
int bq27z758_get_operation_status(i2c_driver_t driver, uint16_t *status);
int bq27z758_get_safety_status(i2c_driver_t driver, uint16_t *status);
int bq27z758_get_manufacturing_status(i2c_driver_t driver, uint16_t *status);
int bq27z758_get_average_time_to_full(i2c_driver_t driver, uint16_t *minutes);
int bq27z758_get_relative_state_of_charge(i2c_driver_t driver, uint16_t *percentage);
int bq27z758_get_state_of_health(i2c_driver_t driver, uint16_t *percentage);
int bq27z758_get_btp_discharge_set(i2c_driver_t driver, uint16_t *btp_discharge_set);
int bq27z758_get_btp_charge_set(i2c_driver_t driver, uint16_t *btp_charge_set);
int bq27z758_get_pf_status(i2c_driver_t driver, uint32_t *status);

int bq27z758_set_btp_discharge_set(i2c_driver_t driver, uint16_t btp_discharge_set);
int bq27z758_set_btp_charge_set(i2c_driver_t driver, uint16_t btp_charge_set);

int bq27z758_toggle_fet_control(i2c_driver_t driver);
int bq27z758_toggle_permanent_failure(i2c_driver_t driver);


#endif
