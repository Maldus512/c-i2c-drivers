#ifndef I2C_DEVICES_GT1151Q_H_INCLUDED
#define I2C_DEVICES_GT1151Q_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"


#define GT1151Q_DEFAULT_ADDRESS  0x28
#define GT1151Q_MAX_TOUCH_POINTS 10


typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t  width;
    uint8_t  height;
} gt1151q_touch_point_t;


int gt1151q_read_touch_points(i2c_driver_t driver, gt1151q_touch_point_t *touch_points, size_t max_touch_points);


#endif
