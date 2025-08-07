#ifndef MIA_M10Q_H_INCLUDED
#define MIA_M10Q_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define MIA_M10Q_DEFAULT_ADDRESS 0x84


struct mia_m10q_pvt_data {
    int32_t longitude;
    int32_t latitude;
};


int mia_m10q_receive_message(i2c_driver_t driver, size_t max_length, uint8_t buffer[max_length]);
int mia_m10q_get_position(i2c_driver_t driver, struct mia_m10q_pvt_data *pvt_data);


#endif
