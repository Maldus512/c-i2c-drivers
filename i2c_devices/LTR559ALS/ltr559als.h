#ifndef LTR559ALS_H_INCLUDED
#define LTR559ALS_H_INCLUDED

#include "../../i2c_common/i2c_common.h"
#include <stdint.h>

typedef enum {
    INTERRUPT_DISABLED = 0,
    INTERRUPT_PS=1,
    INTERRUPT_ALS=2,
    INTERRUPT_PS_ALS=3
} ltr559als_interrupt_mode_t;

int ltr559als_read_info_id(i2c_driver_t driver, uint8_t *p_id, uint8_t *m_id);
int ltr559als_read_als_data(i2c_driver_t driver, uint16_t *ch_0, uint16_t *ch_1);
int ltr559als_als_standby(i2c_driver_t driver, int standby);
int ltr559als_ps_standby(i2c_driver_t driver, int standby);
int ltr559als_read_ps_data(i2c_driver_t driver, uint16_t *value);
int ltr559als_set_interrupt_mode(i2c_driver_t driver, ltr559als_interrupt_mode_t mode);
int ltr559als_set_ps_thresold(i2c_driver_t driver, uint16_t low, uint16_t high);
int ltr559als_read_status(i2c_driver_t driver, uint8_t *buffer);


#endif
