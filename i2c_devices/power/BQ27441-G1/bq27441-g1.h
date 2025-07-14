#ifndef BQ27441_G1_H_INCLUDED
#define BQ27441_G1_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define BQ27441_G1_DEFAULT_ADDRESS 0xAA


int bq27441_g1_get_state_of_charge(i2c_driver_t driver, uint16_t *percentage);


#endif
