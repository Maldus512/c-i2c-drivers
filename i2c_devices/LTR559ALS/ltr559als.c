#include "../../i2c_common/i2c_common.h"
#include <stdint.h>
#include "ltr559als.h"

#define MANUFAC_ID 0x87
#define PART_ID 0x86
#define ALS_DATA_CH1_LOW 0x88
#define ALS_CONTR 0x80
#define PS_CONTR 0x81
#define PS_DATA_0_LOW 0x8d
#define INTERRUPT 0x8f
#define PS_THRES_UP_0 0x90
#define ALS_PS_STATUS 0x8c

int ltr559als_read_info_id(i2c_driver_t driver, uint8_t *p_id, uint8_t *m_id)
{
    uint8_t buffer[2] = {0};
    int res = i2c_read_register(driver, PART_ID, buffer, 2);
    *p_id = buffer[0];
    *m_id = buffer[1];
    return res;
}

int ltr559als_read_als_data(i2c_driver_t driver, uint16_t *ch_0, uint16_t *ch_1)
{
    uint8_t buffer[4] = {0};
    int res = i2c_read_register(driver, ALS_DATA_CH1_LOW, buffer, 4);
    *ch_1 = buffer[0] | (buffer[1] << 8);
    *ch_0 = buffer[2] | (buffer[3] << 8);
    return res;
}

int ltr559als_als_standby(i2c_driver_t driver, int standby)
{
    uint8_t buffer = 0;
    int res = i2c_read_register(driver, ALS_CONTR, &buffer, 1);
    if (res)
        return res;
    if (standby == 0)
        buffer = buffer | 1;
    else
        buffer = buffer & 0xFE;
    return i2c_write_register(driver, ALS_CONTR, &buffer, 1);
}

int ltr559als_ps_standby(i2c_driver_t driver, int standby)
{
    uint8_t buffer = 0;
    int res = i2c_read_register(driver, PS_CONTR, &buffer, 1);
    if (res)
        return res;
    if (standby == 0)
        buffer = buffer | 3;
    else
        buffer = buffer & 0xFC;
    return i2c_write_register(driver, PS_CONTR, &buffer, 1);
}

int ltr559als_read_ps_data(i2c_driver_t driver, uint16_t *value)
{
    uint8_t buffer[2] = {0};
    int res = i2c_read_register(driver, PS_DATA_0_LOW, buffer, 2);
    *value = buffer[0] | ((buffer[1] & 0b111) << 8);
    return res;
}

#include "esp_log.h"

int ltr559als_set_interrupt_mode(i2c_driver_t driver, ltr559als_interrupt_mode_t mode) {

    uint8_t buffer = 0;
    int res = i2c_read_register(driver, INTERRUPT, &buffer, 1);
    if (res) return res;
    buffer=(buffer & 0xfc) | mode;
    i2c_write_register(driver, INTERRUPT, &buffer, 1);
    buffer=0;
    res = i2c_read_register(driver, INTERRUPT, &buffer, 1);
    ESP_LOGI("test", "%x", buffer);
    return 0;
}

int ltr559als_set_ps_thresold(i2c_driver_t driver, uint16_t low, uint16_t high) {

    uint8_t buffer[4]={0};
    buffer[2]=low & 0xff;
    buffer[3]=(low >> 8) & 0b1111;
    buffer[0]=high & 0xff;
    buffer[1]=(high >> 8) & 0b1111;

    return i2c_write_register(driver, PS_THRES_UP_0, buffer, 4);
}

int ltr559als_read_status(i2c_driver_t driver, uint8_t *buffer) {

    return i2c_read_register(driver, ALS_PS_STATUS, buffer, 1);
}



