#include "vcnl4010.h"
#include "esp_log.h"


#define COMMAND_REGISTER                      0x80
#define PRODUCT_ID_REVISION_REGISTER          0x81
#define PROXIMITY_RATE_REGISTER               0x82
#define IR_LED_CURRENT_REGISTER               0x83
#define AMBIENT_LIGHT_PARAMETER_REGISTER      0x84
#define AMBIENT_LIGHT_RESULT_REGISTER_H       0x85
#define AMBIENT_LIGHT_RESULT_REGISTER_L       0x86
#define PROXIMITY_RESULT_REGISTER_H           0x87
#define PROXIMITY_RESULT_REGISTER_L           0x88
#define INTERRUPT_CONTROL_REGISTER            0x89
#define LOW_THRESHOLD_REGISTER_H              0x8A
#define LOW_THRESHOLD_REGISTER_L              0x8B
#define HIGH_THRESHOLD_REGISTER_H             0x8C
#define HIGH_THRESHOLD_REGISTER_L             0x8D
#define INTERRUPT_STATUS_REGISTER             0x8E
#define PROXIMITY_MODULATOR_TIMING_ADJUSTMENT 0x8F

#define ALS_DATA_READY_BIT  6
#define PROX_DATA_READY_BIT 5
#define ALS_OD_BIT          4
#define PROX_OD_BIT         3
#define ALS_EN_BIT          2
#define PROX_EN_BIT         1
#define SELFTIMED_EN_BIT    0

static const char         *TAG        = "vcnl4010";

static int read_command_register(i2c_driver_t driver, uint8_t *command);
static int write_generic_register(i2c_driver_t driver, uint8_t reg, uint8_t *data);
static int command_register_update_bit(i2c_driver_t driver, uint8_t bit, uint8_t value);
static int register_get_bit(i2c_driver_t driver, uint8_t address, uint8_t bit, uint8_t *value);


int vcnl4010_enable_operation(i2c_driver_t driver, uint8_t enable) {
    return command_register_update_bit(driver, SELFTIMED_EN_BIT, enable > 0);
}


int vcnl4010_enable_periodic_proximity_measurement(i2c_driver_t driver, uint8_t enable) {
    return command_register_update_bit(driver, PROX_EN_BIT, enable > 0);
}


int vcnl4010_write_proximity_rate(i2c_driver_t driver, uint8_t value) {
	uint8_t buffer = 0;
	int     res    = i2c_read_register(driver, PROXIMITY_RATE_REGISTER, &buffer, 1);
	if (value <= 7)
	{
		buffer = (buffer & 0xf0) | value;
		return write_generic_register(driver, PROXIMITY_RATE_REGISTER, &buffer);
	}
	return 1;
}

int vcnl4010_write_proximity_current(i2c_driver_t driver, uint8_t value) {
	uint8_t buffer = 0;
	int     res    = i2c_read_register(driver, IR_LED_CURRENT_REGISTER, &buffer, 1);
	if (value <= 20)
	{
		buffer = (buffer & 0xC0) | value;
		ESP_LOGI(TAG, "Current sensor: %d", buffer);
		return write_generic_register(driver, IR_LED_CURRENT_REGISTER, &buffer);
	}
	return 1;
}


int vcnl4010_is_proximity_reading_ready(i2c_driver_t driver, uint8_t *ready) {
    return register_get_bit(driver, COMMAND_REGISTER, PROX_DATA_READY_BIT, ready);
}


int vcnl4010_read_product_id_revision(i2c_driver_t driver, uint8_t *product_id, uint8_t *revision) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, PRODUCT_ID_REVISION_REGISTER, &buffer, 1);
    if (res) {
        return res;
    }

    if (product_id != NULL) {
        *product_id = (buffer & 0xF0) >> 4;
    }

    if (revision != NULL) {
        *revision = buffer & 0xF;
    }

    return 0;
}


int vcnl4010_read_proximity_result(i2c_driver_t driver, uint16_t *result) {
    uint8_t buffer[2] = {0};
    int     res       = i2c_read_register(driver, PROXIMITY_RESULT_REGISTER_H, buffer, sizeof(buffer));
    if (res) {
        return res;
    } else {
        if (result != NULL) {
            *result = (((uint16_t)buffer[0]) << 8) | ((uint16_t)buffer[1]);
        }
        return 0;
    }
}



static int read_command_register(i2c_driver_t driver, uint8_t *command) {
    return i2c_read_register(driver, COMMAND_REGISTER, command, 1);
}


static int write_generic_register(i2c_driver_t driver, uint8_t reg, uint8_t *data) {
    return i2c_write_register(driver, reg, data, 1);
}


static int command_register_update_bit(i2c_driver_t driver, uint8_t bit, uint8_t value) {
    uint8_t command = 0;
    int     res     = read_command_register(driver, &command);
    if (res) {
        return res;
    }

    if (value) {
        command |= (1 << bit);
    } else {
        command &= ~(1 << bit);
    }

    return i2c_write_register(driver, COMMAND_REGISTER, &command, 1);
}


static int register_get_bit(i2c_driver_t driver, uint8_t address, uint8_t bit, uint8_t *value) {
    uint8_t buffer = 0;
    int     res    = i2c_read_register(driver, address, &buffer, 1);
    if (res) {
        return res;
    } else {
        *value = (buffer & (1 << bit)) > 0;
        return 0;
    }
}
