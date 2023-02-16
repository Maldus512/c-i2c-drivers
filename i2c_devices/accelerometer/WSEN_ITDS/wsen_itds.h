#ifndef WSEN_ITDS_H_INCLUDED
#define WSEN_ITDS_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"
#include <stdint.h>

int wsen_itds_init(i2c_driver_t driver);
int wsen_itds_get_full_scale(i2c_driver_t driver, uint8_t *scale);
int wsen_itds_get_coord(i2c_driver_t driver, uint8_t reg_l, uint8_t reg_h,
                        int16_t *coord);
int wsen_itds_get_all_coords(i2c_driver_t driver, int16_t *coord);
int wsen_itds_get_all_coords2(i2c_driver_t driver, int16_t *coord);
int wsen_itds_get_x(i2c_driver_t driver, int16_t *coord);
int wsen_itds_get_y(i2c_driver_t driver, int16_t *coord);
int wsen_itds_get_z(i2c_driver_t driver, int16_t *coord);
int wsen_itds_set_full_scale_2(i2c_driver_t driver);
int wsen_itds_set_full_scale_4(i2c_driver_t driver);
int wsen_itds_set_full_scale_8(i2c_driver_t driver);

#endif
