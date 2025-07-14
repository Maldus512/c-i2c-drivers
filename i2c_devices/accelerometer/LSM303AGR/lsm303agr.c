#include "lsm303agr.h"
#include "../../../i2c_common/i2c_common.h"


#define SENSORS_GRAVITY_EARTH            (9.80665F) /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_MOON             (1.6F)     /**< The moon's gravity in m/s^2 */
#define SENSORS_GRAVITY_SUN              (275.0F)   /**< The sun's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD         (SENSORS_GRAVITY_EARTH)
#define LIS2MDL_MAG_LSB                  1.5F     //!< Sensitivity
#define LIS2MDL_MILLIGAUSS_TO_MICROTESLA 0.1F     //!< Conversion rate of Milligauss to Microtesla

#define STATUS_REG_AUX_A 0x07
#define WHO_AM_I_A       0x0F
#define CTRL_REG_1_A     0x20
#define CTRL_REG_4_A     0x23
#define OUT_X_L_A        0x28
#define OUT_X_H_A        0x29
#define OUT_Y_L_A        0x2A
#define OUT_Y_H_A        0x2B
#define OUT_Z_L_A        0x2C
#define OUT_Z_H_A        0x2D
#define CFG_REG_A_M      0x60
#define CFG_REG_B_M      0x60
#define CFG_REG_C_M      0x62
#define OUTX_L_REG_M     0x68
#define OUTX_H_REG_M     0x69
#define OUTY_L_REG_M     0x6A
#define OUTY_H_REG_M     0x6B
#define OUTZ_L_REG_M     0x6C
#define OUTZ_H_REG_M     0x6D


static uint16_t get_accelerometer_data_shift(lsm303agr_accelerometer_mode_t mode);
int             get_least_significant_bit_value(i2c_driver_t driver, lsm303agr_accelerometer_mode_t mode, float *lsb);


int lsm303agr_get_identifier(i2c_driver_t driver, uint8_t *id) {
    uint8_t request[1] = {WHO_AM_I_A};
    return driver.i2c_transfer(driver.device_address, request, sizeof(request), id, 1, driver.arg);
}


int lsm303agr_get_status(i2c_driver_t driver, uint8_t *status) {
    uint8_t request[1] = {STATUS_REG_AUX_A};
    return driver.i2c_transfer(driver.device_address, request, sizeof(request), status, 1, driver.arg);
}


int lsm303agr_set_enable_axis(i2c_driver_t driver, uint8_t x, uint8_t y, uint8_t z) {
    uint8_t control1 = 0;
    int     res      = i2c_read_register(driver, CTRL_REG_1_A, &control1, 1);
    if (res < 0) {
        return res;
    }

    uint8_t request[2] = {CTRL_REG_1_A, (control1 & 0xF8) | ((x > 0) << 0) | ((y > 0) << 1) | ((z > 0) << 2)};
    return driver.i2c_transfer(driver.device_address, request, sizeof(request), NULL, 0, driver.arg);
}


int lsm303agr_accelerometer_get_scale(i2c_driver_t driver, lsm303agr_accelerometer_scale_t *scale) {
    uint8_t control4 = 0;
    int     res      = i2c_read_register(driver, CTRL_REG_4_A, &control4, 1);
    if (res < 0) {
        return res;
    }

    *scale = (control4 >> 4) & 0x3;
    return 0;
}


int lsm303agr_accelerometer_set_scale(i2c_driver_t driver, lsm303agr_accelerometer_scale_t scale) {
    uint8_t control4 = 0;
    int     res      = i2c_read_register(driver, CTRL_REG_4_A, &control4, 1);
    if (res < 0) {
        return res;
    }

    control4 &= ~(0x3 << 4);
    control4 |= scale << 4;

    return i2c_write_register(driver, CTRL_REG_4_A, &control4, 1);
}


int lsm303agr_accelerometer_get_mode(i2c_driver_t driver, lsm303agr_accelerometer_mode_t *mode) {
    uint8_t control1 = 0;
    int     res      = i2c_read_register(driver, CTRL_REG_1_A, &control1, 1);
    if (res < 0) {
        return res;
    }

    uint8_t control4 = 0;
    res              = i2c_read_register(driver, CTRL_REG_4_A, &control4, 1);
    if (res < 0) {
        return res;
    }

    uint8_t low_power       = (control1 & (1 << 3)) > 0;
    uint8_t high_resolution = (control4 & (1 << 3)) > 0;

    if (low_power) {
        *mode = LSM303AGR_ACCELEROMETER_MODE_LOW_POWER;
    } else if (high_resolution) {
        *mode = LSM303AGR_ACCELEROMETER_MODE_HIGH_RESOLUTION;
    } else {
        *mode = LSM303AGR_ACCELEROMETER_MODE_NORMAL;
    }

    return 0;
}


int lsm303agr_accelerometer_set_mode(i2c_driver_t driver, lsm303agr_accelerometer_mode_t mode) {
    uint8_t low_power       = (mode & 0x02) > 0;
    uint8_t high_resolution = (mode & 0x01) > 0;

    {
        uint8_t control1 = 0;
        int     res      = i2c_read_register(driver, CTRL_REG_1_A, &control1, 1);
        if (res < 0) {
            return res;
        }

        uint8_t request[2] = {CTRL_REG_1_A, (control1 & (~(1 << 3))) | (low_power << 3)};
        res                = driver.i2c_transfer(driver.device_address, request, sizeof(request), NULL, 0, driver.arg);
        if (res < 0) {
            return res;
        }
    }

    {
        uint8_t control4 = 0;
        int     res      = i2c_read_register(driver, CTRL_REG_4_A, &control4, 1);
        if (res < 0) {
            return res;
        }

        uint8_t request[2] = {CTRL_REG_4_A, (control4 & (~(1 << 3))) | (high_resolution << 3)};
        res                = driver.i2c_transfer(driver.device_address, request, sizeof(request), NULL, 0, driver.arg);
        if (res < 0) {
            return res;
        }
    }

    return 0;
}


int lsm303agr_set_accelerometer_odr(i2c_driver_t driver, lsm303agr_accelerometer_odr_t odr) {
    uint8_t control1 = 0;
    int     res      = i2c_read_register(driver, CTRL_REG_1_A, &control1, 1);
    if (res < 0) {
        return res;
    }

    uint8_t request[2] = {CTRL_REG_1_A, (control1 & 0x0F) | ((odr & 0xF) << 4)};
    return driver.i2c_transfer(driver.device_address, request, sizeof(request), NULL, 0, driver.arg);
}


int lsm303agr_read_accelerometer_data(i2c_driver_t driver, lsm303agr_vector_t *data) {
    uint8_t request[6]  = {OUT_X_L_A, OUT_X_H_A, OUT_Y_L_A, OUT_Y_H_A, OUT_Z_L_A, OUT_Z_H_A};
    uint8_t response[6] = {0};

    for (size_t i = 0; i < sizeof(response); i++) {
        int res = driver.i2c_transfer(driver.device_address, &request[i], 1, &response[i], 1, driver.arg);
        if (res < 0) {
            return res;
        }
    }

    int16_t x = response[0] | (response[1] << 8);
    int16_t y = response[2] | (response[3] << 8);
    int16_t z = response[4] | (response[5] << 8);

    lsm303agr_accelerometer_mode_t mode = LSM303AGR_ACCELEROMETER_MODE_NORMAL;
    int                            res  = lsm303agr_accelerometer_get_mode(driver, &mode);
    if (res < 0) {
        return res;
    }

    float lsb = 0;
    res       = get_least_significant_bit_value(driver, mode, &lsb);
    if (res < 0) {
        return res;
    }
    uint16_t shift = get_accelerometer_data_shift(mode);

    data->x = ((float)(x >> shift)) * lsb * SENSORS_GRAVITY_STANDARD;
    data->y = ((float)(y >> shift)) * lsb * SENSORS_GRAVITY_STANDARD;
    data->z = ((float)(z >> shift)) * lsb * SENSORS_GRAVITY_STANDARD;

    return 0;
}


int lsm303agr_set_magnetometer_mode(i2c_driver_t driver, lsm303agr_magnetometer_mode_t mode) {
    uint8_t cfg = 0;
    {
        uint8_t request[1] = {CFG_REG_A_M};
        int     res        = driver.i2c_transfer(driver.device_address, request, sizeof(request), &cfg, 1, driver.arg);
        if (res < 0) {
            return res;
        }
    }

    uint8_t request[2] = {CFG_REG_A_M, (cfg & (~(0x03))) | (mode & 0x3)};
    return driver.i2c_transfer(driver.device_address, request, sizeof(request), NULL, 0, driver.arg);
}


int lsm303agr_set_magnetometer_odr(i2c_driver_t driver, lsm303agr_magnetometer_odr_t odr) {
    uint8_t cfg = 0;
    {
        uint8_t request[1] = {CFG_REG_A_M};
        int     res        = driver.i2c_transfer(driver.device_address, request, sizeof(request), &cfg, 1, driver.arg);
        if (res < 0) {
            return res;
        }
    }

    uint8_t request[2] = {CFG_REG_A_M, (cfg & (~(0x0C))) | ((odr & 0x3) << 2)};
    return driver.i2c_transfer(driver.device_address, request, sizeof(request), NULL, 0, driver.arg);
}


int lsm303agr_magnetometer_set_temperature_compensation(i2c_driver_t driver, uint8_t compensation) {
    uint8_t cfg = 0;
    int     res = i2c_read_register(driver, CFG_REG_A_M, &cfg, 1);
    if (res < 0) {
        return res;
    }

    if (compensation) {
        cfg |= 1 << 8;
    } else {
        cfg &= ~(1 << 8);
    }

    return i2c_write_register(driver, CFG_REG_A_M, &cfg, 1);
}


int lsm303agr_magnetometer_set_offset_cancellation(i2c_driver_t driver, uint8_t cancellation) {
    uint8_t cfg = 0;
    int     res = i2c_read_register(driver, CFG_REG_B_M, &cfg, 1);
    if (res < 0) {
        return res;
    }

    if (cancellation) {
        cfg |= 0x2;
    } else {
        cfg &= ~0x2;
    }

    return i2c_write_register(driver, CFG_REG_B_M, &cfg, 1);
}


int lsm303agr_magnetometer_set_low_pass_filter(i2c_driver_t driver, uint8_t filter) {
    uint8_t cfg = 0;
    int     res = i2c_read_register(driver, CFG_REG_B_M, &cfg, 1);
    if (res < 0) {
        return res;
    }

    if (filter) {
        cfg |= 1;
    } else {
        cfg &= ~1;
    }

    return i2c_write_register(driver, CFG_REG_B_M, &cfg, 1);
}


int lsm303agr_set_magnetometer_bdu(i2c_driver_t driver, uint8_t bdu) {
    uint8_t cfg = 0;
    {
        uint8_t request[1] = {CFG_REG_C_M};
        int     res        = driver.i2c_transfer(driver.device_address, request, sizeof(request), &cfg, 1, driver.arg);
        if (res < 0) {
            return res;
        }
    }

    uint8_t request[2] = {CFG_REG_C_M, (cfg & (~(0x10))) | ((bdu > 0) << 4)};
    return driver.i2c_transfer(driver.device_address, request, sizeof(request), NULL, 0, driver.arg);
}


int lsm303agr_read_magnetometer_data(i2c_driver_t driver, lsm303agr_vector_t *data) {
    uint8_t request[6]  = {OUTX_L_REG_M, OUTX_H_REG_M, OUTY_L_REG_M, OUTY_H_REG_M, OUTZ_L_REG_M, OUTZ_H_REG_M};
    uint8_t response[6] = {0};

    for (size_t i = 0; i < sizeof(response); i++) {
        int res = driver.i2c_transfer(driver.device_address, &request[i], 1, &response[i], 1, driver.arg);
        if (res < 0) {
            return res;
        }
    }

    int16_t x = response[0] | (response[1] << 8);
    int16_t y = response[2] | (response[3] << 8);
    int16_t z = response[4] | (response[5] << 8);

    data->x = ((float)x) * LIS2MDL_MAG_LSB * LIS2MDL_MILLIGAUSS_TO_MICROTESLA;
    data->y = ((float)y) * LIS2MDL_MAG_LSB * LIS2MDL_MILLIGAUSS_TO_MICROTESLA;
    data->z = ((float)z) * LIS2MDL_MAG_LSB * LIS2MDL_MILLIGAUSS_TO_MICROTESLA;

    return 0;
}


static uint16_t get_accelerometer_data_shift(lsm303agr_accelerometer_mode_t mode) {
    switch (mode) {
        case LSM303AGR_ACCELEROMETER_MODE_HIGH_RESOLUTION:
            return 4;
            break;
        case LSM303AGR_ACCELEROMETER_MODE_NORMAL:
            return 6;
        case LSM303AGR_ACCELEROMETER_MODE_LOW_POWER:
            return 8;
        default:     // Invalid
            return 0;
    }

    return 0;
}

int get_least_significant_bit_value(i2c_driver_t driver, lsm303agr_accelerometer_mode_t mode, float *lsb) {
    lsm303agr_accelerometer_scale_t scale = 0;
    int                             res   = lsm303agr_accelerometer_get_scale(driver, &scale);
    if (res < 0) {
        return res;
    }

    switch (mode) {
        case LSM303AGR_ACCELEROMETER_MODE_NORMAL: {
            switch (scale) {
                case LSM303AGR_ACCELEROMETER_SCALE_2G:
                    *lsb = 0.0039;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_4G:
                    *lsb = 0.00782;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_8G:
                    *lsb = 0.01563;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_16G:
                    *lsb = 0.0469;
                    break;
            }
            break;
        }
        case LSM303AGR_ACCELEROMETER_MODE_HIGH_RESOLUTION: {
            switch (scale) {
                case LSM303AGR_ACCELEROMETER_SCALE_2G:
                    *lsb = 0.00098;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_4G:
                    *lsb = 0.00195;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_8G:
                    *lsb = 0.0039;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_16G:
                    *lsb = 0.01172;
                    break;
            }
            break;
        }
        case LSM303AGR_ACCELEROMETER_MODE_LOW_POWER: {
            switch (scale) {
                case LSM303AGR_ACCELEROMETER_SCALE_2G:
                    *lsb = 0.01563;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_4G:
                    *lsb = 0.03126;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_8G:
                    *lsb = 0.06252;
                    break;
                case LSM303AGR_ACCELEROMETER_SCALE_16G:
                    *lsb = 0.18758;
                    break;
            }
            break;
        }
    }

    return 0;
}
