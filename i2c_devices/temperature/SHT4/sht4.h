#ifndef SHT4_H_INCLUDED
#define SHT4_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"


#define SHT4A_DEFAULT_ADDRESS (0x44 << 1)
#define SHT4B_DEFAULT_ADDRESS (0x45 << 1)


/**
 * @brief Milliseconds needed to conclude a sensor read for each corresponding repeatibility level.
 *
 */
#define SHT4_LOW_REPEATABILITY_MS_PERIOD    2
#define SHT4_MEDIUM_REPEATABILITY_MS_PERIOD 5
#define SHT4_HIGH_REPEATABILITY_MS_PERIOD   9


/**
 * @brief Precision values for sensor reads. Higher values will take more time and energy but will yield more
 * accurate results.
 *
 */
typedef enum {
    SHT4_PRECISION_LOW = 0,
    SHT4_PRECISION_MEDIUM,
    SHT4_PRECISION_HIGH,
} sht4_precision_t;


/**
 * @brief Apply a software reset to the IC
 *
 * @param driver
 * @return int
 */
int sht4_soft_reset(i2c_driver_t driver);

/**
 * @brief Start an acquisition (to be later acquired)
 *
 * @param driver
 * @param precision the precision of the measurement.
 * @return int
 */
int sht4_measure(i2c_driver_t driver, sht4_precision_t precision);

/**
 * @brief Acquire a previously started sensor read. If it was not concluded yet the device does not respond; depending
 * on the hardware abstraction layer implementation this may hang the call or fail it
 *
 * @param driver
 * @param temperature Temperature read. May be NULL.
 * @param humidity Humidity read. May be NULL.
 * @return int
 */
int sht4_read(i2c_driver_t driver, double *temperature, double *humidity);

#endif