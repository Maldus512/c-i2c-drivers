#ifndef VCNL4010_H_INCLUDED
#define VCNL4010_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"
#include <stdint.h>


#define VCNL4010_DEFAULT_ADDRESS 0x26


int vcnl4010_enable_operation(i2c_driver_t driver, uint8_t enable);
int vcnl4010_read_product_id_revision(i2c_driver_t driver, uint8_t *product_id, uint8_t *revision);
int vcnl4010_enable_periodic_proximity_measurement(i2c_driver_t driver, uint8_t enable);
int vcnl4010_is_proximity_reading_ready(i2c_driver_t driver, uint8_t *ready);
int vcnl4010_read_proximity_result(i2c_driver_t driver, uint16_t *result);


#endif