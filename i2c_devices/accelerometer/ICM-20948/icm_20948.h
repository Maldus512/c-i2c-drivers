#ifndef ICM_20948_H_INCLUDED
#define ICM_20948_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define ICM_20948_DEFAULT_ADDRESS (0x68 << 1)


typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} icm_20948_axis_data_t;


int icm_20948_get_identifier(i2c_driver_t driver, uint8_t *id);
int icm_20948_read_accelerometer_data(i2c_driver_t driver, icm_20948_axis_data_t *data);


#endif
