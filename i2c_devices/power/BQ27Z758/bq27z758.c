#include "bq27z758.h"


#define COMMAND_MANUFACTURER_ACCESS_LSB     0x00
#define COMMAND_CONTROL_STATUS_LSB          0x00
#define COMMAND_TEMPERATURE                 0x06
#define COMMAND_VOLTAGE_LSB                 0x08
#define COMMAND_BATTERY_STATUS_LSB          0x0A
#define COMMAND_CURRENT_LSB                 0x0C
#define COMMAND_AVERAGE_TIME_TO_FULL        0x18
#define COMMAND_BTP_DISCHARGE_SET           0x24
#define COMMAND_BTP_CHARGE_SET              0x26
#define COMMAND_RELATIVE_STATE_OF_CHARGE    0x2C
#define COMMAND_STATE_OF_HEALTH             0x2E
#define COMMAND_ALT_MANUFACTURER_ACCESS_LSB 0x3E

#define ALT_MANUFACTURER_FET_CONTROL                  0x0022
#define ALT_MANUFACTURER_PERMANENT_FAILURE            0x0024
#define ALT_MANUFACTURER_SAFETY_STATUS_ADDRESS        0x0051
#define ALT_MANUFACTURER_PF_STATUS_ADDRESS            0x0053
#define ALT_MANUFACTURER_OPERATION_STATUS_ADDRESS     0x0054
#define ALT_MANUFACTURER_MANUFACTURING_STATUS_ADDRESS 0x0057


static int read_register(i2c_driver_t driver, uint8_t address, uint16_t *value);
static int write_register(i2c_driver_t driver, uint8_t address, uint16_t value);
static int read_alt_manufacturer_register(i2c_driver_t driver, uint16_t address, uint16_t *value);
static int command_alt_manufacturer(i2c_driver_t driver, uint16_t address);


int bq27z758_toggle_fet_control(i2c_driver_t driver) {
    return command_alt_manufacturer(driver, ALT_MANUFACTURER_FET_CONTROL);
}


int bq27z758_toggle_permanent_failure(i2c_driver_t driver) {
    return command_alt_manufacturer(driver, ALT_MANUFACTURER_PERMANENT_FAILURE);
}


int bq27z758_get_safety_status(i2c_driver_t driver, uint16_t *status) {
    return read_alt_manufacturer_register(driver, ALT_MANUFACTURER_SAFETY_STATUS_ADDRESS, status);
}


int bq27z758_get_manufacturing_status(i2c_driver_t driver, uint16_t *status) {
    /*
     *|  15  |   14  | 13 | 12 | 11 | 10 |  9 |    8   |  7 |  6  |  5  |  4   |    3   |    2   |    1   |  0 |
     *|CAL_EN|LT_TEST|RSVD|RSVD|RSVD|RSVD|RSVD|FET_OVRD|RSVD|PF_EN|LF_EN|FET_EN|GAUGE_EN|DSG_TEST|CHG_TEST|RSVD|
     * CAL_EN (Bit 15): CALIBRATION mode
     *    1 = Enabled
     *    0 = Disabled
     * LT_TEST (Bit 14): LIFETIME SPEED UP mode
     *    1 = Enabled
     *    0 = Disabled
     * RSVD (Bits 13–6): Reserved
     * LF_EN (Bit 5): Lifetime data collection
     *    1 = Enabled
     *    0 = Disabled
     * RSVD (Bit 4): Reserved (but is marked as FET_EN?)
     * GAUGE_EN (Bit 3): Gas gauging
     *    1 = Enabled
     *    0 = Disabled
     * RSVD (Bits 2–0): Reserved
     */
    return read_alt_manufacturer_register(driver, ALT_MANUFACTURER_MANUFACTURING_STATUS_ADDRESS, status);
}


int bq27z758_get_pf_status(i2c_driver_t driver, uint32_t *status) {
    uint8_t command[3] = {COMMAND_ALT_MANUFACTURER_ACCESS_LSB, ALT_MANUFACTURER_PF_STATUS_ADDRESS & 0xFF,
                          (ALT_MANUFACTURER_PF_STATUS_ADDRESS >> 8) & 0xFF};
    uint8_t buffer[36] = {};

    int res = driver.i2c_transfer(driver.device_address, command, sizeof(command), NULL, 0, driver.arg);
    if (res) {
        return res;
    }

    res = driver.i2c_transfer(driver.device_address, command, 1, buffer, sizeof(buffer), driver.arg);
    if (res) {
        return res;
    }

    if ((buffer[0] == (ALT_MANUFACTURER_PF_STATUS_ADDRESS & 0xFF)) &&
        (buffer[1] == ((ALT_MANUFACTURER_PF_STATUS_ADDRESS >> 8) & 0xFF))) {
        *status = (uint32_t)(buffer[5] << 24) | (uint32_t)(buffer[4] << 16) | (uint32_t)(buffer[3] << 8) | buffer[2];
    } else {
        return -1;
    }

    return res;
}


int bq27z758_get_operation_status(i2c_driver_t driver, uint16_t *status) {
    /*
     *| 15  | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7    |  6  |  5 |  4 |  3  |  2 |  1 |  0  |
     *|SLEEP|XCHG|XDSG| PF | SS | SDV|SEC1|SEC0|BTP_INT|SHELF|RSVD|SHIP|ZVCHG|CHG |DSG |SHIPV|
     * SLEEP (Bit 15): SLEEP mode conditions met
     *     1 = Active
     *     0 = Inactive
     * XCHG (Bit 14): Charging disabled
     *     0 = Inactive
     *     1 = Active
     * XDSG (Bit 13): Discharging disabled
     *     1 = Active
     *     0 = Inactive
     * PF (Bit 12): PERMANENT FAILURE mode status
     *     1 = Active
     *     0 = Inactive
     * SS (Bit 11): SAFETY status. This is the ORd value of all the Safety Status bits.
     *     1 = Active
     *     0 = Inactive
     * SDV (Bit 10): SHUTDOWN mode triggered from a low cell voltage
     *     1 = Active
     *     0 = Inactive
     * SEC1, SEC0 (Bits 9, 8): SECURITY mode
     *     0, 0 = Reserved
     *     0, 1 = Full Access
     *     1, 0 = Unsealed
     *     1, 1 = Sealed
     * BTP_INT (Bit 7): Battery Trip Point Interrupt. Setting and clearing this bit depends on various conditions. See
     **Section 8.11 for details. SHELF (Bit 6): SHELF mode 1 = Active 0 = Inactive RSVD (Bit 5): Reserved SHIP (Bit 4):
     *SHIP mode 1 = Active 0 = Inactive ZVCHG (Bit 3): Zero-volt (low voltage) charging status 1 = Active 0 = Inactive
     * CHG (Bit 2): CHG FET status
     *     1 = Active
     *     0 = Inactive
     * DSG (Bit 1): DSG FET status
     *     1 = Active
     *     0 = Inactive
     * SHIPV (Bit 0): SHIP mode triggered via low cell voltage
     *     1 = Active
     *     0 = Inactive
     */
    return read_alt_manufacturer_register(driver, ALT_MANUFACTURER_OPERATION_STATUS_ADDRESS, status);
}


int bq27z758_get_control_status(i2c_driver_t driver, uint16_t *status) {
    /*
     *| 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
     *|RSVD|FAS | SS |RSVD|RSVD|RSVD|RSVD|RSVD|RSVD|RSVD|RSVD|RSVD|LDMD|RDIS|VOK |QEN |
     *
     * RSVD (Bit 15): Reserved
     * FAS, SS (Bits 14–13): Legacy SECURITY mode
     *    0, 0 = Full Access
     *    0, 1 = Reserved
     *    1, 0 = Unsealed
     *    1, 1 = Sealed
     * RSVD (Bit 12): Reserved
     * RSVD (Bits 11–10): Reserved
     * CheckSumValid (Bit 9): Checksum valid
     *    1 = Flash Writes are enabled.
     *    0 = Flash Writes are disabled due to low voltage or PF condition.
     * RSVD (Bits 8–4): Reserved
     * LDMD (Bit 3): LOAD mode
     *    1 = Constant power
     *    0 = Constant current
     * RDIS (Bit 2): Resistance updates
     *    1 = Disabled
     *    0 = Enabled
     * VOK (Bit 1): Voltage OK for QMax update
     *    1 = Detected
     *    0 = Not detected
     * QEN (Bit 0): Impedance Track Gauging (Ra and QMax updates are enabled.)
     */
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


int bq27z758_get_btp_discharge_set(i2c_driver_t driver, uint16_t *btp_discharge_set) {
    return read_register(driver, COMMAND_BTP_DISCHARGE_SET, btp_discharge_set);
}


int bq27z758_get_btp_charge_set(i2c_driver_t driver, uint16_t *btp_charge_set) {
    return read_register(driver, COMMAND_BTP_CHARGE_SET, btp_charge_set);
}


int bq27z758_get_state_of_health(i2c_driver_t driver, uint16_t *percentage) {
    return read_register(driver, COMMAND_STATE_OF_HEALTH, percentage);
}


int bq27z758_get_relative_state_of_charge(i2c_driver_t driver, uint16_t *percentage) {
    return read_register(driver, COMMAND_RELATIVE_STATE_OF_CHARGE, percentage);
}


int bq27z758_get_battery_status(i2c_driver_t driver, uint16_t *status) {
    /*
     * | 15 | 14 | 13 | 12 | 11 | 10 |  9 | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
     * |RSVD|TCA |RSVD|RSVD|TDA |RSVD|RCA |RSVD|INIT|DSG | FC | FD |RSVD|RSVD|RSVD|RSVD|
     *
     * RSVD (Bit 15): Reserved
     * TCA (Bit 14): Terminate Charge Alarm
     *     0 = Inactive
     *     1 = Active
     * RSVD (Bits 13–12): Reserved
     * TDA (Bit 11): Terminate Discharge Alarm
     *     0 = Inactive
     *     1 = Active
     * RSVD (Bit 10): Reserved
     * RCA (Bit 9): Remaining Capacity Alarm
     *     0 = Inactive
     *     1 = Active
     * RSVD (Bit 8): Reserved
     * INIT (Bit 7): Initialization
     *     0 = Complete
     *     1 = Active
     * DSG (Bit 6): Discharging
     *     0 = The battery is charging.
     *     1 = The battery is discharging.
     * FC (Bit 5): Fully Charged
     *     0 = The battery is not fully charged.
     *     1 = The battery is fully charged.
     * FD (Bit 4): Fully Discharged
     *     0 = The battery is okay.
     *     1 = The battery is fully depleted.
     * RSVD (Bits 3–0): Reserved
     */
    return read_register(driver, COMMAND_BATTERY_STATUS_LSB, status);
}


int bq27z758_get_voltage(i2c_driver_t driver, uint16_t *voltage) {
    return read_register(driver, COMMAND_VOLTAGE_LSB, voltage);
}


int bq27z758_get_current(i2c_driver_t driver, int16_t *current) {
    return read_register(driver, COMMAND_CURRENT_LSB, current);
}


int bq27z758_set_btp_discharge_set(i2c_driver_t driver, uint16_t btp_discharge_set) {
    return write_register(driver, COMMAND_BTP_DISCHARGE_SET, btp_discharge_set);
}


int bq27z758_set_btp_charge_set(i2c_driver_t driver, uint16_t btp_charge_set) {
    return write_register(driver, COMMAND_BTP_CHARGE_SET, btp_charge_set);
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


static int write_register(i2c_driver_t driver, uint8_t address, uint16_t value) {
    uint8_t command[3] = {address, value & 0xFF, (value >> 8) & 0xFF};
    return driver.i2c_transfer(driver.device_address, command, sizeof(command), NULL, 0, driver.arg);
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
