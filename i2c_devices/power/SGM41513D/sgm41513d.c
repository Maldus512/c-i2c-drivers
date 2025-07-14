#include "sgm41513d.h"
#include "../../../i2c_common/i2c_common.h"


#define REG01 0x01
#define REG07 0x07
#define REG09 0x09


int sgm41513d_get_charging_stat(i2c_driver_t driver, sgm41513d_charging_stat_t *stat) {
    uint8_t command[] = {0x08};
    uint8_t buffer[1] = {0};

    int res = driver.i2c_transfer(driver.device_address, command, sizeof(command), buffer, sizeof(buffer), driver.arg);
    *stat   = (buffer[0] >> 3) & 0x3;

    return res;
}


int sgm41513d_get_faults(i2c_driver_t driver, uint8_t *faults) {
    uint8_t command[] = {REG07};
    uint8_t buffer[1] = {0};

    int res = driver.i2c_transfer(driver.device_address, command, sizeof(command), buffer, sizeof(buffer), driver.arg);
    *faults = buffer[0];

    return res;
}


int sgm41513d_reset_watchdog(i2c_driver_t driver) {
    uint8_t reg01 = 0;

    int res = i2c_read_register(driver, REG01, &reg01, 1);
    if (res < 0) {
        return res;
    }

    reg01 |= 1 << 6;

    return i2c_write_register(driver, REG01, &reg01, 1);
}


int sgm41513d_enable_charging(i2c_driver_t driver, uint8_t enable) {
    uint8_t reg01 = 0;

    int res = i2c_read_register(driver, REG01, &reg01, 1);
    if (res < 0) {
        return res;
    }

    if (enable) {
        reg01 |= 1 << 4;
    } else {
        reg01 &= ~(1 << 4);
    }

    return i2c_write_register(driver, REG01, &reg01, 1);
}


int sgm41513d_configure_batfet(i2c_driver_t driver, uint8_t enable, uint8_t delay) {
    uint8_t reg07 = 0;

    int res = i2c_read_register(driver, REG07, &reg07, 1);
    if (res < 0) {
        return res;
    }

    if (delay) {
        reg07 |= 1 << 3;
    } else {
        reg07 &= ~(1 << 3);
    }

    if (enable) {
        reg07 &= ~(1 << 5);
    } else {
        reg07 |= 1 << 5;
    }

    return i2c_write_register(driver, REG07, &reg07, 1);
}
