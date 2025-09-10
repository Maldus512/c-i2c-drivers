#include "bq24157.h"


int bq27441_g1_get_state_of_charge(i2c_driver_t driver, uint16_t *percentage) {
    uint8_t command[] = {0x1C};
    uint8_t buffer[2] = {0};

    int res = driver.i2c_transfer(driver.device_address, command, sizeof(command), buffer, sizeof(buffer), driver.arg);
    *percentage = (uint16_t)((buffer[1] << 8) | buffer[0]);

    return res;
}
