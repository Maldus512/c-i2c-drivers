#ifndef LTR559ALS_H_INCLUDED
#define LTR559ALS_H_INCLUDED

#include "../../i2c_common/i2c_common.h"
#include <stdint.h>

#define LTR_STATUS_ALS_DATA (1<<2)

typedef enum {
    INTERRUPT_DISABLED = 0,
    INTERRUPT_PS=1,
    INTERRUPT_ALS=2,
    INTERRUPT_PS_ALS=3
} ltr559als_interrupt_mode_t;

//NEW 

typedef enum {
    ALS_GAIN_1X = 0,
    ALS_GAIN_2X=1,
    ALS_GAIN_4X=2,
    ALS_GAIN_8X=3,
    ALS_GAIN_48X=6,
    ALS_GAIN_96X=7
}ltr559als_als_gain_t;

typedef enum {
    PS_MEAS_RATE_50MS = 0,
    PS_MEAS_RATE_70MS = 1,
    PS_MEAS_RATE_100MS = 2,
    PS_MEAS_RATE_200MS = 3,
    PS_MEAS_RATE_500MS = 4,
    PS_MEAS_RATE_1000MS = 5,
    PS_MEAS_RATE_2000MS = 6,
    PS_MEAS_RATE_10MS = 8
} ltr559als_ps_meas_rate_t;

typedef enum {
    ALS_MEAS_RATE_50MS = 0,
    ALS_MEAS_RATE_100MS = 1,
    ALS_MEAS_RATE_200MS = 2,
    ALS_MEAS_RATE_500MS = 3,
    ALS_MEAS_RATE_1000MS = 4,
    ALS_MEAS_RATE_2000MS = 5
} ltr559als_als_meas_rate_t;

typedef enum {
    ALS_INTEGRATION_TIME_100MS = 0,
    ALS_INTEGRATION_TIME_50MS = 1,
    ALS_INTEGRATION_TIME_200MS = 2,
    ALS_INTEGRATION_TIME_400MS = 3,
    ALS_INTEGRATION_TIME_150MS = 4,
    ALS_INTEGRATION_TIME_250MS = 5,
    ALS_INTEGRATION_TIME_300MS = 6,
    ALS_INTEGRATION_TIME_350MS = 7
} ltr559als_als_integration_time_t;

typedef enum {
    INTERRUPT_PERSIST_1=0,
    INTERRUPT_PERSIST_2=1,
    INTERRUPT_PERSIST_3=2,
    INTERRUPT_PERSIST_4=3,
    INTERRUPT_PERSIST_5=4,
    INTERRUPT_PERSIST_6=5,
    INTERRUPT_PERSIST_7=6,
    INTERRUPT_PERSIST_8=7,
    INTERRUPT_PERSIST_9=8,
    INTERRUPT_PERSIST_10=9,
    INTERRUPT_PERSIST_11=10,
    INTERRUPT_PERSIST_12=11,
    INTERRUPT_PERSIST_13=12,
    INTERRUPT_PERSIST_14=13,
    INTERRUPT_PERSIST_15=14,
    INTERRUPT_PERSIST_16=15
} ltr559als_interrupt_persist_t;

int ltr559als_read_info_id(i2c_driver_t driver, uint8_t *p_id, uint8_t *m_id);
int ltr559als_read_als_data(i2c_driver_t driver, uint16_t *ch_0, uint16_t *ch_1);
int ltr559als_als_standby(i2c_driver_t driver, int standby);
int ltr559als_ps_standby(i2c_driver_t driver, int standby);
int ltr559als_read_ps_data(i2c_driver_t driver, uint16_t *value);
int ltr559als_set_interrupt_mode(i2c_driver_t driver, ltr559als_interrupt_mode_t mode);
int ltr559als_set_ps_thresold(i2c_driver_t driver, uint16_t low, uint16_t high);
int ltr559als_read_status(i2c_driver_t driver, uint8_t *buffer);


//NEW 
int ltr559als_set_als_gain(i2c_driver_t driver, ltr559als_als_gain_t mode);
int ltr559als_ps_saturation_indicator_en(i2c_driver_t driver, int enable);
int ltr559als_set_ps_meas_rate(i2c_driver_t driver, ltr559als_ps_meas_rate_t mode);
int ltr559als_set_als_meas_rate(i2c_driver_t driver, ltr559als_als_meas_rate_t mode);
int ltr559als_set_als_integration_time(i2c_driver_t driver, ltr559als_als_integration_time_t mode);
int ltr559als_set_als_data_gain(i2c_driver_t driver, ltr559als_als_gain_t mode);
int ltr559als_set_interrupt_polarity(i2c_driver_t driver, int polarity);
int ltr559als_set_interrupt_persist(i2c_driver_t driver, ltr559als_interrupt_persist_t als_mode, ltr559als_interrupt_persist_t ps_mode);
int ltr559als_set_ps_interrupt_persist(i2c_driver_t driver, ltr559als_interrupt_persist_t mode);
int ltr559als_set_als_interrupt_persist(i2c_driver_t driver, ltr559als_interrupt_persist_t mode);

int ltr559als_read_visible_spectrum(i2c_driver_t driver, uint16_t *light);
#endif
