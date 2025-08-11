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


int zephyr_i2c_port_scan(const struct device *i2c_dev) {
    printk("\n    | 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f |\n");
    printk("----|---------------------------------------------------------------------------------");

    uint8_t        error = 0u;
    uint8_t        dst;
    uint8_t        i2c_dev_cnt = 0;
    struct i2c_msg msgs[1];
    msgs[0].buf   = &dst;
    msgs[0].len   = 1U;
    msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

    /* Use the full range of I2C address for display purpose */
    for (uint16_t x = 0; x <= 0x7f; x++) {
        /* New line every 0x10 address */
        if (x % 0x10 == 0) {
            printk("|\n0x%02x| ", x);
        }
        /* Range the test with the start and stop value configured in the kconfig */
        if (x >= 8 && x <= 127) {
            /* Send the address to read from */
            error = i2c_transfer(i2c_dev, &msgs[0], 1, x);
            /* I2C device found on current address */
            if (error == 0) {
                printk("0x%02x ", x);
                i2c_dev_cnt++;
            } else {
                printk(" --  ");
            }
        } else {
            /* Scan value out of range, not scanned */
            printk("     ");
        }
    }
    printk("|\n");
    printk("\nI2C device(s) found on the bus: %d\nScanning done.\n\n", i2c_dev_cnt);

    return 0;
}
