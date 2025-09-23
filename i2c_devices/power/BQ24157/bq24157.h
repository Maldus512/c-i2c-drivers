#ifndef BQ24157_H_INCLUDED
#define BQ24157_H_INCLUDED


#include <stdbool.h>
#include "../../../i2c_common/i2c_common.h"


#define BQ24157_DEFAULT_ADDRESS (0x6A << 1)


enum bq24157_current_limit {
    BQ24157_CURRENT_LIMIT_100MA = 0,
    BQ24157_CURRENT_LIMIT_500MA = 1,
    BQ24157_CURRENT_LIMIT_800MA = 2,
    BQ24157_CURRENT_LIMIT_NONE  = 3,
};

enum bq24157_charge_state {
    BQ24157_CHARGE_STATE_READY = 0,
    BQ24157_CHARGE_IN_PROGRESS = 1,
    BQ24157_CHARGE_DONE        = 2,
    BQ24157_CHARGE_FAULT       = 3,
};

struct bq24157_status {
    enum bq24157_charge_state charge_state;
    bool                      boost;
    uint8_t                   fault;
};


int bq24157_get_status(i2c_driver_t driver, struct bq24157_status *status);
int bq24157_set_control(i2c_driver_t driver);


#endif
