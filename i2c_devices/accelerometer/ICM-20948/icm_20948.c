#include "icm_20948.h"


#define WHO_AM_I     0x00
#define ACCEL_XOUT_H 0x2D


int icm_20948_get_identifier(i2c_driver_t driver, uint8_t *id) {
    uint8_t request[1] = {WHO_AM_I};
    return driver.i2c_transfer(driver.device_address, request, sizeof(request), id, 1, driver.arg);
}


int icm_20948_read_accelerometer_data(i2c_driver_t driver, icm_20948_axis_data_t *data) {
    uint8_t request[1]  = {ACCEL_XOUT_H};
    uint8_t response[6] = {0};

    int res =
        driver.i2c_transfer(driver.device_address, request, sizeof(request), response, sizeof(response), driver.arg);

    data->x = response[1] | (response[0] << 8);
    data->y = response[3] | (response[2] << 8);
    data->z = response[5] | (response[4] << 8);

    return res;
}
