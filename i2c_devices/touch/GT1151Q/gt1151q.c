#include "gt1151q.h"

#define STATUS_REGISTER_ADDR                      0x814E
#define STATUS_REGISTER_READY(x)                  ((x & 0x80) > 0)
#define STATUS_REGISTER_NUMBER_OF_TOUCH_POINTS(x) (x & 0x0F)

#define POINT_COORDINATE_REGISTER_BASE_ADDR      0x8150
#define POINT_COORDINATE_REGISTER_N_BASE_ADDR(n) (POINT_COORDINATE_REGISTER_BASE_ADDR + n * 8)


static int read_register(i2c_driver_t driver, uint16_t address, uint8_t *data, size_t len);
static int write_register(i2c_driver_t driver, uint16_t address, uint8_t register_content);


int gt1151q_read_touch_points(i2c_driver_t driver, gt1151q_touch_point_t *touch_points, size_t max_touch_points) {
    uint8_t status_register_content = 0;
    if (read_register(driver, STATUS_REGISTER_ADDR, &status_register_content, 1)) {
        return -1;
    }

    if (STATUS_REGISTER_READY(status_register_content)) {
        // TODO: check checksum
        size_t number_of_touch_points = STATUS_REGISTER_NUMBER_OF_TOUCH_POINTS(status_register_content);
        number_of_touch_points = number_of_touch_points > max_touch_points ? max_touch_points : number_of_touch_points;

        uint8_t touch_points_buffer[GT1151Q_MAX_TOUCH_POINTS * 8] = {0};
        if (read_register(driver, POINT_COORDINATE_REGISTER_BASE_ADDR, touch_points_buffer,
                          number_of_touch_points * 8)) {
            return -1;
        }

        for (size_t i = 0; i < number_of_touch_points; i++) {
            touch_points[i].x      = touch_points_buffer[i * 8] | (((uint16_t)touch_points_buffer[i * 8 + 1]) << 8);
            touch_points[i].y      = touch_points_buffer[i * 8 + 2] | (((uint16_t)touch_points_buffer[i * 8 + 3]) << 8);
            touch_points[i].width  = touch_points_buffer[i * 8 + 4];
            touch_points[i].height = touch_points_buffer[i * 8 + 5];
        }

        // Clear the status register
        if (write_register(driver, STATUS_REGISTER_ADDR, 0)) {
            return -1;
        } else {
            return number_of_touch_points;
        }
    } else {
        return 0;
    }
}


static int read_register(i2c_driver_t driver, uint16_t address, uint8_t *data, size_t len) {
    uint8_t buf[2] = {(address >> 8) & 0xFF, address & 0xFF};
    return driver.i2c_transfer(driver.device_address, buf, sizeof(buf), data, len, driver.arg);
}


static int write_register(i2c_driver_t driver, uint16_t address, uint8_t register_content) {
    uint8_t buffer[3] = {(address >> 8) & 0xFF, address & 0xFF, register_content};
    return driver.i2c_transfer(driver.device_address, buffer, sizeof(buffer), NULL, 0, driver.arg);
}
