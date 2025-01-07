#ifndef SHT3_H_INCLUDED
#define SHT3_H_INCLUDED

#include "../../../i2c_common/i2c_common.h"


#define SHT3_DEFAULT_ADDRESS (0x44 << 1)


/**
 * @brief Milliseconds needed to conclude a sensor read for each corresponding repeatibility level.
 *
 */
#define SHT3_LOW_REPEATABILITY_MS_PERIOD    5
#define SHT3_MEDIUM_REPEATABILITY_MS_PERIOD 7
#define SHT3_HIGH_REPEATABILITY_MS_PERIOD   16


/**
 * @brief Repeatability values for sensor reads. Higher values will take more time and energy but will yield more
 * accurate results.
 *
 */
typedef enum {
    SHT3_LOW_REPEATABILITY = 0,
    SHT3_MEDIUM_REPEATABILITY,
    SHT3_HIGH_REPEATABILITY,
} sht3_repeatability_t;


/**
 * @brief Start a single acquisition (to be later acquired)
 *
 * @param driver
 * @param clock_stretching Whether to use clock stretching.
 * @param repeatibility the repeatibility of the measurement.
 * @return int
 */
int sht3_start_single_acquisition(i2c_driver_t driver, int clock_stretching, sht3_repeatability_t repeatibility);

/**
 * @brief Acquire a previously started sensor read. If it was not concluded yet the device does not respond; depending
 * on the hardware abstraction layer implementation this may hang the call or fail it
 *
 * @param driver
 * @param temperature Temperature read. May be NULL.
 * @param humidity Humidity read. May be NULL.
 * @return int
 */
int sht3_read_acquisition(i2c_driver_t driver, double *temperature, double *humidity);

/**
 * @brief Read the status register
 *
 * @param driver
 * @param status Pointer where the status register content will be stored. Must not be NULL.
 * @return int
 */
int sht3_read_status_register(i2c_driver_t driver, uint16_t *status);
int sht3_disable_heater(i2c_driver_t driver);
int sht3_soft_reset(i2c_driver_t driver);

#endif
