#ifndef LSM303AGR_H_INCLUDED
#define LSM303AGR_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define LSM303AGR_DEFAULT_ACCELEROMETER_ADDRESS (0x19 << 1)
#define LSM303AGR_DEFAULT_MAGNETOMETER_ADDRESS  (0x1E << 1)


typedef struct {
    float x;
    float y;
    float z;
} lsm303agr_vector_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} lsm303agr_raw_vector_t;

typedef enum {
    LSM303AGR_ACCELEROMETER_MODE_LOW_POWER       = 0x02,
    LSM303AGR_ACCELEROMETER_MODE_NORMAL          = 0x00,
    LSM303AGR_ACCELEROMETER_MODE_HIGH_RESOLUTION = 0x01,
} lsm303agr_accelerometer_mode_t;

typedef enum {
    LSM303AGR_ACCELEROMETER_SCALE_2G  = 0,
    LSM303AGR_ACCELEROMETER_SCALE_4G  = 1,
    LSM303AGR_ACCELEROMETER_SCALE_8G  = 2,
    LSM303AGR_ACCELEROMETER_SCALE_16G = 3,
} lsm303agr_accelerometer_scale_t;

typedef enum {
    LSM303AGR_ACCELEROMETER_ODR_POWER_DOWN       = 0,
    LSM303AGR_ACCELEROMETER_ODR_1HZ              = 1,
    LSM303AGR_ACCELEROMETER_ODR_10HZ             = 2,
    LSM303AGR_ACCELEROMETER_ODR_25HZ             = 3,
    LSM303AGR_ACCELEROMETER_ODR_50HZ             = 4,
    LSM303AGR_ACCELEROMETER_ODR_100HZ            = 5,
    LSM303AGR_ACCELEROMETER_ODR_200HZ            = 6,
    LSM303AGR_ACCELEROMETER_ODR_400HZ            = 7,
    LSM303AGR_ACCELEROMETER_ODR_1_620KHZ         = 8,
    LSM303AGR_ACCELEROMETER_ODR_1_344KHZ_5_376HZ = 9,
} lsm303agr_accelerometer_odr_t;

typedef enum {
    LSM303AGR_MAGNETOMETER_ODR_10HZ  = 0,
    LSM303AGR_MAGNETOMETER_ODR_20HZ  = 1,
    LSM303AGR_MAGNETOMETER_ODR_50HZ  = 2,
    LSM303AGR_MAGNETOMETER_ODR_100HZ = 3,
} lsm303agr_magnetometer_odr_t;

typedef enum {
    LSM303AGR_MAGNETOMETER_MODE_CONTINUOUS = 0,
    LSM303AGR_MAGNETOMETER_MODE_SINGLE     = 1,
    LSM303AGR_MAGNETOMETER_MODE_IDLE1      = 2,
    LSM303AGR_MAGNETOMETER_MODE_IDLE2      = 3,
} lsm303agr_magnetometer_mode_t;


int lsm303agr_get_status(i2c_driver_t driver, uint8_t *status);
int lsm303agr_get_identifier(i2c_driver_t driver, uint8_t *id);
int lsm303agr_set_enable_axis(i2c_driver_t driver, uint8_t x, uint8_t y, uint8_t z);
int lsm303agr_set_accelerometer_odr(i2c_driver_t driver, lsm303agr_accelerometer_odr_t odr);
int lsm303agr_read_accelerometer_data(i2c_driver_t driver, lsm303agr_vector_t *data);
int lsm303agr_accelerometer_get_mode(i2c_driver_t driver, lsm303agr_accelerometer_mode_t *mode);
int lsm303agr_accelerometer_set_mode(i2c_driver_t driver, lsm303agr_accelerometer_mode_t mode);
int lsm303agr_accelerometer_get_scale(i2c_driver_t driver, lsm303agr_accelerometer_scale_t *scale);
int lsm303agr_accelerometer_set_scale(i2c_driver_t driver, lsm303agr_accelerometer_scale_t scale);

int lsm303agr_set_magnetometer_mode(i2c_driver_t driver, lsm303agr_magnetometer_mode_t mode);
int lsm303agr_set_magnetometer_odr(i2c_driver_t driver, lsm303agr_magnetometer_odr_t odr);
int lsm303agr_set_magnetometer_bdu(i2c_driver_t driver, uint8_t bdu);
int lsm303agr_read_magnetometer_data(i2c_driver_t driver, lsm303agr_vector_t *data, lsm303agr_raw_vector_t *raw_data);
int lsm303agr_magnetometer_set_temperature_compensation(i2c_driver_t driver, uint8_t compensation);
int lsm303agr_magnetometer_set_low_pass_filter(i2c_driver_t driver, uint8_t filter);
int lsm303agr_magnetometer_set_offset_cancellation(i2c_driver_t driver, uint8_t cancellation);
int lsm303agr_magnetometer_set_hard_iron_calbration(i2c_driver_t driver, lsm303agr_raw_vector_t calibration_data);
int lsm303agr_magnetometer_get_configuration_registers(i2c_driver_t driver, uint8_t registers[static 3]);


#endif
