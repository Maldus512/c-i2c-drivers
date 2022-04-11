#include <stdint.h>
#include "../../../i2c_common/i2c_common.h"
#include "sht3.h"

#define SINGLE_SHOT_CLOCK_STRETCHING_MSB    0x2C
#define SINGLE_SHOT_NO_CLOCK_STRETCHING_MSB 0x24

#define SINGLE_SHOT_CLOCK_STRETCHING_LSB_HIGH_REP    0x06
#define SINGLE_SHOT_CLOCK_STRETCHING_LSB_MED_REP     0x0D
#define SINGLE_SHOT_CLOCK_STRETCHING_LSB_LOW_REP     0x10
#define SINGLE_SHOT_NO_CLOCK_STRETCHING_LSB_HIGH_REP 0x00
#define SINGLE_SHOT_NO_CLOCK_STRETCHING_LSB_MED_REP  0x0B
#define SINGLE_SHOT_NO_CLOCK_STRETCHING_LSB_LOW_REP  0x16


#define CMD_SOFT_RESET           0x30A2
#define CMD_HEATER_DISABLED      0x3066
#define CMD_HEATER_ENABLE        0x306D
#define CMD_READ_STATUS_REGISTER 0xF32D


static int     send_command(i2c_driver_t driver, uint16_t command);
static uint8_t sht3_crc8(uint8_t *data, size_t len);


int sht3_read_status_register(i2c_driver_t driver, uint16_t *status) {
    uint8_t writebuf[2] = {(CMD_READ_STATUS_REGISTER >> 8) & 0xFF, CMD_READ_STATUS_REGISTER & 0xFF};
    uint8_t readbuf[2]  = {0};

    int res = driver.i2c_transfer(driver.device_address, writebuf, 2, readbuf, 2, driver.arg);
    *status = (((uint16_t)readbuf[0]) << 8) | ((uint16_t)readbuf[1]);

    return res;
}


int sht3_disable_heater(i2c_driver_t driver) {
    return send_command(driver, CMD_HEATER_DISABLED);
}


int sht3_soft_reset(i2c_driver_t driver) {
    return send_command(driver, CMD_SOFT_RESET);
}


int sht3_start_single_acquisition(i2c_driver_t driver, int clock_stretching, sht3_repeatability_t repeatibility) {
    if (repeatibility > SHT3_HIGH_REPEATABILITY)
        return -1;

    uint8_t msbs[]    = {SINGLE_SHOT_NO_CLOCK_STRETCHING_MSB, SINGLE_SHOT_CLOCK_STRETCHING_MSB};
    uint8_t lsbs[][3] = {
        {SINGLE_SHOT_NO_CLOCK_STRETCHING_LSB_LOW_REP, SINGLE_SHOT_NO_CLOCK_STRETCHING_LSB_MED_REP,
         SINGLE_SHOT_NO_CLOCK_STRETCHING_LSB_HIGH_REP},
        {SINGLE_SHOT_CLOCK_STRETCHING_LSB_LOW_REP, SINGLE_SHOT_CLOCK_STRETCHING_LSB_MED_REP,
         SINGLE_SHOT_CLOCK_STRETCHING_LSB_HIGH_REP},
    };

    int     index    = clock_stretching ? 1 : 0;
    uint8_t buffer[] = {msbs[index], lsbs[index][repeatibility]};

    return driver.i2c_transfer(driver.device_address, buffer, 2, NULL, 0, driver.arg);
}


int sht3_read_acquisition(i2c_driver_t driver, double *temperature, double *humidity) {
    uint8_t  buffer[6];
    uint16_t temp, hum;

    int res = driver.i2c_transfer(driver.device_address, NULL, 0, buffer, 6, driver.arg);

    if (res)
        return res;

    if (sht3_crc8(&buffer[0], 2) != buffer[2] || sht3_crc8(&buffer[3], 2) != buffer[5])
        return 1;

    temp = (buffer[0] << 8) | buffer[1];
    hum  = (buffer[3] << 8) | buffer[4];

    if (temperature)
        *temperature = -45 + 175 * (((double)temp) / ((double)(0x10000 - 1)));
    if (humidity)
        *humidity = 100 * (((double)hum) / ((double)(0x10000 - 1)));

    return 0;
}


static int send_command(i2c_driver_t driver, uint16_t command) {
    uint8_t writebuf[] = {(command >> 8) & 0xFF, command & 0xFF};
    return driver.i2c_transfer(driver.device_address, writebuf, sizeof(writebuf), NULL, 0, driver.arg);
}


static uint8_t sht3_crc8(uint8_t *data, size_t len) {
    uint8_t polynomial = 0x31;
    uint8_t crc        = 0xFF;
    size_t  i, j;
    for (j = len; j; j--) {
        crc ^= *data++;
        for (i = 8; i; i--) {
            crc = (crc & 0x80) ? (crc << 1) ^ polynomial : (crc << 1);
        }
    }
    return crc;
}
