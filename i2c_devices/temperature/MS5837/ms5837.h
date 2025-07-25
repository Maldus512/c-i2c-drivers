#ifndef MS5837_H_INCLUDED
#define MS5837_H_INCLUDED

#include <stdint.h>
#include "../../../i2c_common/i2c_common.h"


#define MS5837_DEFAULT_ADDRESS (0x76 << 1)


typedef enum {
    MS5837_OSR_256  = 0x0,
    MS5837_OSR_512  = 0x2,
    MS5837_OSR_1024 = 0x4,
    MS5837_OSR_2048 = 0x6,
    MS5837_OSR_4096 = 0x8,
    MS5837_OSR_8192 = 0xA,
} ms5837_oversampling_ratio_t;


typedef struct {
    uint16_t factory_data;
    uint16_t pressure_sensitivity;
    uint16_t pressure_offset;
    uint16_t temperature_coefficient_pressure_sensitivity;
    uint16_t temperature_coefficient_pressure_offset;
    uint16_t reference_temperature;
    uint16_t temperature_coefficient_temperature;
} ms5837_prom_t;


int  ms5837_init(i2c_driver_t driver, ms5837_prom_t *prom);
int  ms5837_read_pressure_adc(i2c_driver_t driver, ms5837_oversampling_ratio_t osr, uint32_t *adc);
int  ms5837_read_temperature_adc(i2c_driver_t driver, ms5837_oversampling_ratio_t osr, uint32_t *adc);
void ms5837_calculate(ms5837_prom_t prom, uint32_t adc_temperature, uint32_t adc_pressure, float *temperature,
                      float *pressure);


#endif
