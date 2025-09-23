#include "bq27z758.h"


#define COMMAND_MANUFACTURER_ACCESS_LSB     0x00
#define COMMAND_CONTROL_STATUS_LSB          0x00
#define COMMAND_TEMPERATURE                 0x06
#define COMMAND_VOLTAGE_LSB                 0x08
#define COMMAND_BATTERY_STATUS_LSB          0x0A
#define COMMAND_CURRENT_LSB                 0x0C
#define COMMAND_AVERAGE_TIME_TO_FULL        0x18
#define COMMAND_RELATIVE_STATE_OF_CHARGE    0x2C
#define COMMAND_STATE_OF_HEALTH             0x2E
#define COMMAND_ALT_MANUFACTURER_ACCESS_LSB 0x3E

#define ALT_MANUFACTURER_FET_CONTROL                  0x0022
#define ALT_MANUFACTURER_SAFETY_STATUS_ADDRESS        0x0051
#define ALT_MANUFACTURER_OPERATION_STATUS_ADDRESS     0x0054
#define ALT_MANUFACTURER_MANUFACTURING_STATUS_ADDRESS 0x0057


static int read_register(i2c_driver_t driver, uint8_t address, uint16_t *value);
static int read_alt_manufacturer_register(i2c_driver_t driver, uint16_t address, uint16_t *value);
static int command_alt_manufacturer(i2c_driver_t driver, uint16_t address);


int bq27z758_toggle_fet_control(i2c_driver_t driver) {
    return command_alt_manufacturer(driver, ALT_MANUFACTURER_FET_CONTROL);
}


int bq27z758_get_safety_status(i2c_driver_t driver, uint16_t *status) {
    return read_alt_manufacturer_register(driver, ALT_MANUFACTURER_SAFETY_STATUS_ADDRESS, status);
}


int bq27z758_get_manufacturing_status(i2c_driver_t driver, uint16_t *status) {
    return read_alt_manufacturer_register(driver, ALT_MANUFACTURER_MANUFACTURING_STATUS_ADDRESS, status);
}


int bq27z758_get_operation_status(i2c_driver_t driver, uint16_t *status) {
    return read_alt_manufacturer_register(driver, ALT_MANUFACTURER_OPERATION_STATUS_ADDRESS, status);
}


int bq27z758_get_control_status(i2c_driver_t driver, uint16_t *status) {
    return read_register(driver, COMMAND_CONTROL_STATUS_LSB, status);
}


int bq27z758_get_temperature(i2c_driver_t driver, int16_t *temperature_celsius) {
    uint16_t temperature_kelvin = 0;
    int      res                = read_register(driver, COMMAND_TEMPERATURE, &temperature_kelvin);

    *temperature_celsius = temperature_kelvin - 2731;

    return res;
}


int bq27z758_get_average_time_to_full(i2c_driver_t driver, uint16_t *minutes) {
    return read_register(driver, COMMAND_AVERAGE_TIME_TO_FULL, minutes);
}


int bq27z758_get_state_of_health(i2c_driver_t driver, uint16_t *percentage) {
    return read_register(driver, COMMAND_STATE_OF_HEALTH, percentage);
}


int bq27z758_get_relative_state_of_charge(i2c_driver_t driver, uint16_t *percentage) {
    return read_register(driver, COMMAND_RELATIVE_STATE_OF_CHARGE, percentage);
}


int bq27z758_get_battery_status(i2c_driver_t driver, uint16_t *status) {
    return read_register(driver, COMMAND_BATTERY_STATUS_LSB, status);
}


int bq27z758_get_voltage(i2c_driver_t driver, uint16_t *voltage) {
    return read_register(driver, COMMAND_VOLTAGE_LSB, voltage);
}


int bq27z758_get_current(i2c_driver_t driver, int16_t *current) {
    return read_register(driver, COMMAND_CURRENT_LSB, current);
}


static int read_register(i2c_driver_t driver, uint8_t address, uint16_t *value) {
    uint8_t command[] = {address};
    uint8_t buffer[2] = {0};

    int res = driver.i2c_transfer(driver.device_address, command, sizeof(command), buffer, sizeof(buffer), driver.arg);
    if (res == 0) {
        *value = (uint16_t)((buffer[1] << 8) | buffer[0]);
    }

    return res;
}


static int command_alt_manufacturer(i2c_driver_t driver, uint16_t address) {
    uint8_t command[3] = {COMMAND_ALT_MANUFACTURER_ACCESS_LSB, address & 0xFF, (address >> 8) & 0xFF};
    return driver.i2c_transfer(driver.device_address, command, sizeof(command), NULL, 0, driver.arg);
}


static int read_alt_manufacturer_register(i2c_driver_t driver, uint16_t address, uint16_t *value) {
    uint8_t command[3] = {COMMAND_ALT_MANUFACTURER_ACCESS_LSB, address & 0xFF, (address >> 8) & 0xFF};
    uint8_t buffer[36] = {};

    int res = driver.i2c_transfer(driver.device_address, command, sizeof(command), NULL, 0, driver.arg);
    if (res) {
        return res;
    }

    res = driver.i2c_transfer(driver.device_address, command, 1, buffer, sizeof(buffer), driver.arg);
    if (res) {
        return res;
    }

    if ((buffer[0] == (address & 0xFF)) && (buffer[1] == ((address >> 8) & 0xFF))) {
        *value = (uint16_t)((buffer[3] << 8) | buffer[2]);
    } else {
        return -1;
    }

    return res;
}


static int write_data_flash(i2c_driver_t driver, uint16_t address, size_t length, uint8_t data[length]) {
    uint8_t command[32 + 3] = {COMMAND_ALT_MANUFACTURER_ACCESS_LSB, address & 0xFF, (address >> 8) & 0xFF};

    if (length > 32) {
        return -1;
    }

    memcpy(&command[3], data, length);

    int res = driver.i2c_transfer(driver.device_address, command, 3 + length, NULL, 0, driver.arg);

    return res;
}
