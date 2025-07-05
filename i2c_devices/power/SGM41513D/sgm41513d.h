#ifndef SGM41513D_H_INCLUDED
#define SGM41513D_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define SGM41513D_DEFAULT_ADDRESS (0x1A << 1)


typedef enum {
    SGM41513D_CHARGING_STAT_CHARGE_DISABLE = 0,
    SGM41513D_CHARGING_STAT_PRE_CHARGE,
    SGM41513D_CHARGING_STAT_FAST_CHARGING,
    SGM41513D_CHARGING_STAT_CHARGE_TERMINATED,
} sgm41513d_charging_stat_t;


int sgm41513d_get_charging_stat(i2c_driver_t driver, sgm41513d_charging_stat_t *stat);
int sgm41513d_reset_watchdog(i2c_driver_t driver);
int sgm41513d_enable_charging(i2c_driver_t driver, uint8_t enable);
int sgm41513d_get_faults(i2c_driver_t driver, uint8_t *faults);
int sgm41513d_configure_batfet(i2c_driver_t driver, uint8_t enable, uint8_t delay);


#endif
