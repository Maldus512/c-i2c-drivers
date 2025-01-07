#include <stdint.h>
#include "../../../i2c_common/i2c_common.h"
#include "sht4.h"

#define CMD_HIGH_PRECISION_READ   0xFD
#define CMD_MEDIUM_PRECISION_READ 0xF6
#define CMD_LOW_PRECISION_READ    0xE0
#define CMD_SOFT_RESET            0x94

#define SHT4_CRC8(data, len) i2c_common_crc8(0x31, 0xFF, data, len)


static int send_command(i2c_driver_t driver, uint8_t command);


int sht4_soft_reset(i2c_driver_t driver) {
    return send_command(driver, CMD_SOFT_RESET);
}


int sht4_measure(i2c_driver_t driver, sht4_precision_t precision) {
    if (precision > SHT4_PRECISION_HIGH) {
        return -1;
    }

    const uint8_t command[] = {CMD_LOW_PRECISION_READ, CMD_MEDIUM_PRECISION_READ, CMD_HIGH_PRECISION_READ};
    uint8_t       buffer[1] = {command[precision]};

    return driver.i2c_transfer(driver.device_address, buffer, sizeof(buffer), NULL, 0, driver.arg);
}


int sht4_read(i2c_driver_t driver, double *temperature, double *humidity) {
    uint8_t  buffer[6];
    uint16_t temp, hum;

    int res = driver.i2c_transfer(driver.device_address, NULL, 0, buffer, 6, driver.arg);

    if (res) {
        return res;
    }

    if (SHT4_CRC8(&buffer[0], 2) != buffer[2] || SHT4_CRC8(&buffer[3], 2) != buffer[5]) {
        return 1;
    }

    temp = (buffer[0] << 8) | buffer[1];
    hum  = (buffer[3] << 8) | buffer[4];

    if (temperature) {
        *temperature = -45 + 175 * (((double)temp) / ((double)(0x10000 - 1)));
    }
    if (humidity) {
        *humidity = 100 * (((double)hum) / ((double)(0x10000 - 1)));
    }

    return 0;
}


static int send_command(i2c_driver_t driver, uint8_t command) {
    uint8_t writebuf[] = {command};
    return driver.i2c_transfer(driver.device_address, writebuf, sizeof(writebuf), NULL, 0, driver.arg);
}
