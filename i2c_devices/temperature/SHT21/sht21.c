#include "../../../i2c_common/i2c_common.h"
#include "sht21.h"

#define TRIGGER_T_MEASUREMENT  0xE3
#define TRIGGER_RH_MEASUREMENT 0xE5
#define WRITE_USER_REGISTER    0xE6
#define READ_USER_REGISTER     0xE7
#define SOFT_RESET             0xFE
#define NO_HOLD_MASTER_BIT     0x10


static uint8_t sht21_crc(uint8_t *data, uint8_t len) {
    // CRC
    // const u16t POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001

    uint8_t bit, crc;

    crc = 0;

    // calculates 8-Bit checksum with given polynomial
    for (int i = 0; i < len; i++) {
        crc ^= (data[i]);
        for (bit = 8; bit > 0; --bit) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x131;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}


int sht21_soft_reset(i2c_driver_t driver) {
    uint8_t buffer = SOFT_RESET;
    return driver.i2c_transfer(driver.device_address, &buffer, 1, NULL, 0, driver.arg);
}


int sht21_read_user_register(i2c_driver_t driver, uint8_t *ureg) {
    int     res      = 0;
    uint8_t wrbuf[1] = {READ_USER_REGISTER};

    res = driver.i2c_transfer(driver.device_address, wrbuf, 1, ureg, 1, driver.arg);

    return res;
}


int sht21_write_user_register(i2c_driver_t driver, uint8_t ureg) {
    int     res      = 0;
    uint8_t wrbuf[2] = {WRITE_USER_REGISTER, ureg};

    res = driver.i2c_transfer(driver.device_address, wrbuf, 2, NULL, 0, driver.arg);

    return res;
}


int sht21_set_resolution(i2c_driver_t driver, sht21_resolution_t resolution) {
    int     res = 0;
    uint8_t ureg;

    if (resolution != SHT21_RESOLUTION_12_14 && resolution != SHT21_RESOLUTION_10_13 &&
        resolution != SHT21_RESOLUTION_11_11 && resolution != SHT21_RESOLUTION_8_12)
        return -1;

    if ((res = sht21_read_user_register(driver, &ureg)))
        return res;

    ureg &= (~SHT21_RESOLUTION_MASK);
    ureg |= resolution;

    return sht21_write_user_register(driver, ureg);
}


int sht21_read(i2c_driver_t driver, double *temperature, double *humidity, unsigned long msdelay) {
    int          res       = 0;
    unsigned int counter   = 0;
    uint8_t      buffer[4] = {0};

    msdelay = msdelay > 100 ? 100 : msdelay;     // no point in waiting more than 100ms

    if (temperature != NULL) {
        buffer[0] = TRIGGER_T_MEASUREMENT | NO_HOLD_MASTER_BIT;
        res = driver.i2c_transfer(driver.device_address, buffer, 1, NULL, 0, driver.arg);
        if (res) {
            return res;
        }

        do {
            driver.delay_ms(msdelay);
            res = driver.i2c_transfer(driver.device_address, NULL, 0, buffer, 3, driver.arg);
            counter += msdelay;
        } while (res && counter < 100);

        if (res) {
            return res;
        }

        if (sht21_crc(buffer, 2) != buffer[2]) {
            return -1;
        } else {
            uint16_t rawt = (((uint16_t)buffer[0] << 8) | (buffer[1] & (~0x3)));
            *temperature  = (-46.85 + 175.72 / 65536.0 * (float)(rawt));
        }
    }

    if (humidity != NULL) {
        buffer[0] = TRIGGER_RH_MEASUREMENT | NO_HOLD_MASTER_BIT;
        driver.i2c_transfer(driver.device_address, buffer, 1, NULL, 0, driver.arg);

        do {
            driver.delay_ms(5);
            res = driver.i2c_transfer(driver.device_address, NULL, 0, buffer, 3, driver.arg);
        } while (res && counter++ < 200);

        if (sht21_crc(buffer, 2) != buffer[2]) {
            return -1;
        } else {
            uint16_t rawrh = (((uint16_t)buffer[0] << 8) | (buffer[1] & (~0x3)));
            *humidity      = (-6 + 125 / 65536.0 * (double)rawrh);
        }
    }

    return res;
}