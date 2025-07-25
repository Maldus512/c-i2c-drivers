#include "../../../i2c_common/i2c_common.h"
#include "ms5837.h"


#define RESET_CMD           0x1E
#define CONVERT_D1_CMD(osr) (0x40 | (osr & 0xF))
#define CONVERT_D2_CMD(osr) (0x50 | (osr & 0xF))
#define ADC_READ_CMD        0x00
#define PROM_READ_CMD(i)    (0xA0 | ((i << 1) & 0xE))

#define BASE_C_TEMPERATURE 2000


static int     read_adc(i2c_driver_t driver, uint8_t cmd, int delay, uint32_t *adc);
static uint8_t crc4(uint16_t n_prom[]);


static const int ms_delays[] = {
    [MS5837_OSR_256] = 1,  [MS5837_OSR_512] = 2,  [MS5837_OSR_1024] = 3,
    [MS5837_OSR_2048] = 5, [MS5837_OSR_4096] = 9, [MS5837_OSR_8192] = 18,
};


int ms5837_init(i2c_driver_t driver, ms5837_prom_t *prom) {
    uint8_t buffer[] = {RESET_CMD};
    int     res      = driver.i2c_transfer(driver.device_address, buffer, sizeof(buffer), NULL, 0, driver.arg);
    if (res != 0) {
        return res;
    }

    uint16_t prom_content[8] = {0};
    size_t   i               = 0;

    for (i = 0; i < 7; i++) {
        uint8_t buffer[]     = {PROM_READ_CMD(i)};
        uint8_t read_data[2] = {0};
        res = driver.i2c_transfer(driver.device_address, buffer, sizeof(buffer), read_data, sizeof(read_data),
                                  driver.arg);
        if (res != 0) {
            return res;
        }

        prom_content[i] = (((uint16_t)read_data[0]) << 8) | (read_data[1]);
    }

    prom->factory_data                                 = prom_content[0];
    prom->pressure_sensitivity                         = prom_content[1];
    prom->pressure_offset                              = prom_content[2];
    prom->temperature_coefficient_pressure_sensitivity = prom_content[3];
    prom->temperature_coefficient_pressure_offset      = prom_content[4];
    prom->reference_temperature                        = prom_content[5];
    prom->temperature_coefficient_temperature          = prom_content[6];

    uint8_t crc_read = (prom_content[0] >> 12) & 0xF;
    uint8_t crc_calc = crc4(prom_content);

    if (crc_read != crc_calc) {
        return -1;
    } else {
        return 0;
    }
}


int ms5837_read_pressure_adc(i2c_driver_t driver, ms5837_oversampling_ratio_t osr, uint32_t *adc) {
    return read_adc(driver, CONVERT_D1_CMD(osr), ms_delays[osr], adc);
}


int ms5837_read_temperature_adc(i2c_driver_t driver, ms5837_oversampling_ratio_t osr, uint32_t *adc) {
    return read_adc(driver, CONVERT_D2_CMD(osr), ms_delays[osr], adc);
}


void ms5837_calculate(ms5837_prom_t prom, uint32_t adc_temperature, uint32_t adc_pressure, float *temperature,
                      float *pressure) {
    volatile int32_t dT = ((int32_t)adc_temperature) - ((int32_t)(((uint32_t)prom.reference_temperature) << 8));

    volatile int32_t first_order_temperature =
        (int32_t)(BASE_C_TEMPERATURE +
                  (((int64_t)dT) * ((int64_t)prom.temperature_coefficient_temperature)) / 8388608 /* 2^23 */);


    volatile int64_t OFF =
        (((int64_t)prom.pressure_offset) << 17) + (((int64_t)prom.temperature_coefficient_pressure_offset) * dT) / 64;
    volatile int64_t SENS = (((int64_t)prom.pressure_sensitivity) << 16) +
                            (((int64_t)prom.temperature_coefficient_pressure_sensitivity) * dT) / 128;
    // int32_t first_order_pressure = (int32_t)(((((uint64_t)adc_pressure) * SENS) / (1ULL << 21) - OFF) / (1ULL <<
    // 15));

    volatile int64_t Ti    = 0;
    volatile int64_t OFFi  = 0;
    volatile int64_t SENSi = 0;

    if (first_order_temperature < BASE_C_TEMPERATURE) {
        Ti    = (11 * (((int64_t)dT) ^ 2)) / 34359738368 /* 2^35 */;
        OFFi  = (31 * ((first_order_temperature - BASE_C_TEMPERATURE) ^ 2)) / 8;
        SENSi = (63 * ((first_order_temperature - BASE_C_TEMPERATURE) ^ 2)) / 32;
    }

    volatile int64_t OFF2  = OFF - OFFi;
    volatile int64_t SENS2 = SENS - SENSi;

    int64_t second_order_temperature = first_order_temperature - Ti;
    int64_t second_order_pressure    = (((adc_pressure * SENS2) / 2097152 /* 2^21 */) - OFF2) / (1ULL << 15);

    if (temperature != NULL) {
        *temperature = ((float)second_order_temperature) / 100.f;
    }

    if (pressure != NULL) {
        *pressure = ((float)second_order_pressure) / 100.f;
    }
}


static int read_adc(i2c_driver_t driver, uint8_t cmd, int delay, uint32_t *adc) {
    uint8_t convert_buffer[] = {cmd};
    int res = driver.i2c_transfer(driver.device_address, convert_buffer, sizeof(convert_buffer), NULL, 0, driver.arg);
    if (res != 0) {
        return res;
    }

    driver.delay_ms(delay);

    uint8_t adc_buffer[]   = {ADC_READ_CMD};
    uint8_t read_buffer[3] = {0};
    res = driver.i2c_transfer(driver.device_address, adc_buffer, sizeof(adc_buffer), read_buffer, sizeof(read_buffer),
                              driver.arg);

    *adc = (((uint32_t)read_buffer[0]) << 16) | (((uint32_t)read_buffer[1]) << 8) | read_buffer[2];
    return res;
}


// n_prom defined as 8x unsigned int (n_prom[8])
static uint8_t crc4(uint16_t n_prom[]) {
    size_t   cnt;           // simple counter
    uint16_t n_rem = 0;     // crc remainder
    size_t   n_bit;

    n_prom[0] = ((n_prom[0]) & 0x0FFF);     // CRC byte is replaced by 0
    n_prom[7] = 0;                          // Subsidiary value, set to 0
    for (cnt = 0; cnt < 16; cnt++)          // operation is performed on bytes
    {                                       // choose LSB or MSB
        if (cnt % 2 == 1) {
            n_rem ^= (uint16_t)((n_prom[cnt >> 1]) & 0x00FF);
        } else {
            n_rem ^= (uint16_t)(n_prom[cnt >> 1] >> 8);
        }
        for (n_bit = 8; n_bit > 0; n_bit--) {
            if (n_rem & (0x8000)) {
                n_rem = (n_rem << 1) ^ 0x3000;
            } else {
                n_rem = (n_rem << 1);
            }
        }
    }
    n_rem = ((n_rem >> 12) & 0x000F);     // final 4-bit remainder is CRC code
    return (n_rem ^ 0x00);
}
