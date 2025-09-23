#ifndef DRV2605L_H_INCLUDED
#define DRV2605L_H_INCLUDED


#include <stdbool.h>
#include "../../../i2c_common/i2c_common.h"


#define DRV2605L_DEFAULT_ADDRESS (0x5A << 1)


enum drv2605l_mode {
    DRV2605L_MODE_INTERNAL_TRIGGER            = 0,
    DRV2605L_MODE_EXTERNAL_TRIGGER_EDGE_MODE  = 1,
    DRV2605L_MODE_EXTERNAL_TRIGGER_LEVEL_MODE = 2,
    DRV2605L_MODE_PWM                         = 3,
    DRV2605L_MODE_AUDIO_TO_VIBE               = 4,
    DRV2605L_MODE_REAL_TIME_PLAYBACK          = 5,
    DRV2605L_MODE_DIAGNOSTICS                 = 6,
    DRV2605L_MODE_AUTO_CALIBRATION            = 7,
};


enum drv2605l_motor_type {
    DRV2605L_MOTOR_TYPE_ERM = 0,
    DRV2605L_MOTOR_TYPE_LRA = 1,
};


enum drv2605l_data_format_rtp {
    DRV2605L_DATA_FORMAT_RTP_SIGNED   = 0,
    DRV2605L_DATA_FORMAT_RTP_UNSIGNED = 1,
};


int drv2605l_set_standby(i2c_driver_t driver, bool active);
int drv2605l_set_mode(i2c_driver_t driver, enum drv2605l_mode mode);
int drv2605l_set_motor_type(i2c_driver_t driver, enum drv2605l_motor_type motor_type);
int drv2605l_set_erm_open_loop(i2c_driver_t driver, bool open);
int drv2605l_set_lra_open_loop(i2c_driver_t driver, bool open);
int drv2605l_set_data_format_rtp(i2c_driver_t driver, enum drv2605l_data_format_rtp data_format_rtp);
int drv2605l_set_rtp(i2c_driver_t driver, uint8_t rtp);


#endif
