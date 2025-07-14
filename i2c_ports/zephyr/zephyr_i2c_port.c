#include <zephyr/drivers/i2c.h>
#include "zephyr_i2c_port.h"


int zephyr_i2c_port_transfer(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf, size_t readlen,
                             void *arg) {
    struct i2c_msg msg[2] = {0};
    size_t         i      = 0;

    if (writebuf && writelen > 0) {
        msg[i].buf   = writebuf;
        msg[i].len   = writelen;
        msg[i].flags = I2C_MSG_WRITE;

        if (!readbuf || readlen == 0) {
            msg[i].flags |= I2C_MSG_STOP;
        }
        i++;
    }

    if (readbuf && readlen > 0) {
        msg[i].buf   = readbuf;
        msg[i].len   = readlen;
        msg[i].flags = I2C_MSG_READ | I2C_MSG_STOP;
        if (i > 0) {
            msg[i].flags |= I2C_MSG_RESTART;
        }
        i++;
    }

    const struct device *dev = arg;
    return i2c_transfer(dev, msg, i, devaddr >> 1);
}
