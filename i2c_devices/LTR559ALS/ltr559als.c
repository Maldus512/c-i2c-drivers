#include "../../i2c_common/i2c_common.h"
#include <stdint.h>
#include "ltr559als.h"

#define ALS_CONTR         0x80
#define PS_CONTR          0x81
#define PS_MEAS_RATE      0x84
#define ALS_MEAS_RATE     0x85
#define PART_ID           0x86
#define MANUFAC_ID        0x87
#define ALS_DATA_CH1_LOW  0x88
#define ALS_PS_STATUS     0x8C
#define PS_DATA_0_LOW     0x8D
#define INTERRUPT         0x8F
#define PS_THRES_UP_0     0x90
#define PS_OFFSET_0       0x94
#define INTERRUPT_PERSIST 0x9E


int ltr559als_read_info_id(i2c_driver_t driver, uint8_t *p_id, uint8_t *m_id) {
    uint8_t buffer[2] = {0};
    int     res       = i2c_read_register(driver, PART_ID, buffer, 2);
    *p_id             = buffer[0];
    *m_id             = buffer[1];
    return res;
}


int ltr559als_read_als_data(i2c_driver_t driver, uint16_t *ch_0, uint16_t *ch_1) {
    uint8_t buffer[4] = {0};
    int     res       = i2c_read_register(driver, ALS_DATA_CH1_LOW, buffer, 4);
    if (ch_1)
        *ch_1 = buffer[0] | (buffer[1] << 8);
    if (ch_0)
        *ch_0 = buffer[2] | (buffer[3] << 8);
    return res;
}


int ltr559als_read_visible_spectrum(i2c_driver_t driver, uint16_t *light) {
    uint16_t ch_1, ch_0;
    int      res = ltr559als_read_als_data(driver, &ch_0, &ch_1);
    if (ch_0 < ch_1)
        *light = 0;
    else
        *light = ch_0 - ch_1;
    return res;
}


int ltr559als_als_standby(i2c_driver_t driver, int standby) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, ALS_CONTR, &buffer, 1);
    if (res)
        return res;
    if (standby == 0)
        buffer = buffer | 1;
    else
        buffer = buffer & 0xFE;
    return i2c_write_register(driver, ALS_CONTR, &buffer, 1);
}


int ltr559als_ps_standby(i2c_driver_t driver, int standby) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, PS_CONTR, &buffer, 1);
    if (res)
        return res;
    if (standby == 0)
        buffer = buffer | 3;
    else
        buffer = buffer & 0xFC;
    return i2c_write_register(driver, PS_CONTR, &buffer, 1);
}


int ltr559als_read_ps_data(i2c_driver_t driver, uint16_t *value) {
    uint8_t buffer[2] = {0};
    int     res       = i2c_read_register(driver, PS_DATA_0_LOW, buffer, 2);
    *value            = buffer[0] | ((buffer[1] & 0b111) << 8);
    return res;
}


int ltr559als_set_interrupt_mode(i2c_driver_t driver, ltr559als_interrupt_mode_t mode) {

    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, INTERRUPT, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0xfc) | mode;
    i2c_write_register(driver, INTERRUPT, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, INTERRUPT, &buffer, 1);
    return 0;
}


int ltr559als_set_ps_thresold(i2c_driver_t driver, uint16_t low, uint16_t high) {
    uint8_t buffer[4] = {0};

    buffer[2] = low & 0xff;
    buffer[3] = (low >> 8) & 0b1111;
    buffer[0] = high & 0xff;
    buffer[1] = (high >> 8) & 0b1111;

    return i2c_write_register(driver, PS_THRES_UP_0, buffer, 4);
}


int ltr559als_set_ps_offset(i2c_driver_t driver, uint16_t offset) {
    uint8_t buffer[2] = {0};

    buffer[0] = (offset & 0x300) >> 8;
    buffer[1] = offset & 0xFF;

    return i2c_write_register(driver, PS_OFFSET_0, buffer, 2);
}


int ltr559als_read_status(i2c_driver_t driver, uint8_t *buffer) {
    return i2c_read_register(driver, ALS_PS_STATUS, buffer, 1);
}


int ltr559als_set_als_gain(i2c_driver_t driver, ltr559als_als_gain_t mode) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, ALS_CONTR, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0xe3) | (mode << 2);
    i2c_write_register(driver, ALS_CONTR, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, ALS_CONTR, &buffer, 1);
    return 0;
}


int ltr559als_ps_saturation_indicator_en(i2c_driver_t driver, int enable) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, PS_CONTR, &buffer, 1);
    if (res)
        return res;
    if (enable == 0)
        buffer = buffer & 0xdf;
    else if (enable == 1)
        buffer = buffer | 0x20;
    return i2c_write_register(driver, PS_CONTR, &buffer, 1);
}


int ltr559als_set_ps_meas_rate(i2c_driver_t driver, ltr559als_ps_meas_rate_t mode) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, PS_MEAS_RATE, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0xf0) | mode;
    i2c_write_register(driver, PS_MEAS_RATE, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, PS_MEAS_RATE, &buffer, 1);
    return 0;
}


int ltr559als_set_als_meas_rate(i2c_driver_t driver, ltr559als_als_meas_rate_t mode) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, ALS_MEAS_RATE, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0xf8) | mode;
    i2c_write_register(driver, ALS_MEAS_RATE, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, ALS_MEAS_RATE, &buffer, 1);
    return 0;
}


// integration time must be <= als_meas_rate
int ltr559als_set_als_integration_time(i2c_driver_t driver, ltr559als_als_integration_time_t mode) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, ALS_MEAS_RATE, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0xc7) | (mode << 3);
    i2c_write_register(driver, ALS_MEAS_RATE, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, ALS_MEAS_RATE, &buffer, 1);
    return 0;
}


int ltr559als_set_als_data_gain(i2c_driver_t driver, ltr559als_als_gain_t mode) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, ALS_PS_STATUS, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0x8f) | (mode << 4);
    i2c_write_register(driver, ALS_PS_STATUS, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, ALS_PS_STATUS, &buffer, 1);
    return 0;
}


int ltr559als_set_interrupt_polarity(i2c_driver_t driver, int polarity) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, INTERRUPT, &buffer, 1);
    if (res)
        return res;
    if (polarity == 0)
        buffer = buffer & 0xfb;
    else if (polarity == 1)
        buffer = buffer | 0x4;
    return i2c_write_register(driver, INTERRUPT, &buffer, 1);
}


int ltr559als_set_interrupt_persist(i2c_driver_t driver, ltr559als_interrupt_persist_t als_mode,
                                    ltr559als_interrupt_persist_t ps_mode) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0) | als_mode | (ps_mode << 4);
    i2c_write_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    return 0;
}


// funzioni spezzate
int ltr559als_set_ps_interrupt_persist(i2c_driver_t driver, ltr559als_interrupt_persist_t mode) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0xf) | (mode << 4);
    i2c_write_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    return 0;
}


int ltr559als_set_als_interrupt_persist(i2c_driver_t driver, ltr559als_interrupt_persist_t mode) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    if (res)
        return res;
    buffer = (buffer & 0xf0) | mode;
    i2c_write_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    buffer = 0;
    res    = i2c_read_register(driver, INTERRUPT_PERSIST, &buffer, 1);
    return 0;
}