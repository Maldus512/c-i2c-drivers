#include "bq24157.h"
#include "../../../i2c_common/i2c_common.h"


#define STATUS_CONTROL_REGISTER                            0
#define CONTROL_REGISTER                                   1
#define CONTROL_BATTERY_VOLTAGE_REGISTER                   2
#define BATTERY_TERMINATION_FAST_CHARGE_CURRENT_REGISTER   4
#define SPECIAL_CHARGER_VOLTAGE_ENABLE_PIN_STATUS_REGISTER 5


int bq24157_get_status(i2c_driver_t driver, struct bq24157_status *status) {
    uint8_t result = 0;
    int     res    = i2c_read_register(driver, STATUS_CONTROL_REGISTER, &result, 1);

    status->charge_state = (result >> 4) & 0x3;
    status->boost        = result & (1 << 3);
    status->fault        = result & 0x7;

    return res;
}


int bq24157_set_control(i2c_driver_t driver) {
    uint8_t control = 0x70;
    i2c_write_register(driver, CONTROL_REGISTER, &control, 1);

    control = 0x8B;
    i2c_write_register(driver, CONTROL_BATTERY_VOLTAGE_REGISTER, &control, 1);

    control = 0x11;
    i2c_write_register(driver, BATTERY_TERMINATION_FAST_CHARGE_CURRENT_REGISTER, &control, 1);

    control = 0x03;
    i2c_write_register(driver, SPECIAL_CHARGER_VOLTAGE_ENABLE_PIN_STATUS_REGISTER, &control, 1);

    return 0;
}
