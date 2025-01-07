#include "shtc3.h"


#define COMMAND_SLEEP                                                0xB098
#define COMMAND_WAKEUP                                               0x3517
#define COMMAND_SOFTWARE_RESET                                       0x805D
#define COMMAND_NORMAL_MEASUREMENT_CLOCK_STRETCHING_DISABLED_T_FIRST 0x7866


static int     send_command(i2c_driver_t driver, uint16_t command);
static uint8_t shtc3_crc8(uint8_t *data, size_t len);


int shtc3_sleep(i2c_driver_t driver) {
    return send_command(driver, COMMAND_SLEEP);
}


int shtc3_wakeup(i2c_driver_t driver) {
    return send_command(driver, COMMAND_WAKEUP);
}


int shtc3_reset(i2c_driver_t driver) {
    return send_command(driver, COMMAND_SOFTWARE_RESET);
}


int shtc3_start_temperature_humidity_measurement(i2c_driver_t driver) {
    return send_command(driver, COMMAND_NORMAL_MEASUREMENT_CLOCK_STRETCHING_DISABLED_T_FIRST);
}

int shtc3_read_temperature_humidity_measurement(i2c_driver_t driver, int16_t *temperature, int16_t *humidity) {
    uint8_t buffer[6] = {0};

    int res = driver.i2c_transfer(driver.device_address, NULL, 0, buffer, sizeof(buffer), driver.arg);
    if (res) {
        return res;
    }

    uint8_t crc_calc = shtc3_crc8(buffer, 2);     // Temperature CRC check
    if (crc_calc != buffer[2]) {
        return -1;
    }

    crc_calc = shtc3_crc8(&buffer[3], 2);     // Humidity CRC check
    if (crc_calc != buffer[5]) {
        return -1;
    }

    if (temperature != NULL) {
        uint32_t raw_temperature = (buffer[0] << 8) | buffer[1];
        *temperature             = (int16_t)(-45 + (int32_t)((175 * raw_temperature) / (1 << 16)));
    }

    if (humidity != NULL) {
        uint32_t raw_humidity = (buffer[3] << 8) | buffer[4];
        *humidity             = (int16_t)((100 * raw_humidity) / (1 << 16));
    }

    return 0;
}


static int send_command(i2c_driver_t driver, uint16_t command) {
    uint8_t buffer[2] = {(command >> 8) & 0xFF, command & 0xFF};
    return driver.i2c_transfer(driver.device_address, buffer, 2, NULL, 0, driver.arg);
}


static uint8_t shtc3_crc8(uint8_t *data, size_t len) {
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
