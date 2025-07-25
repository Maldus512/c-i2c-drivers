#ifndef ZEPHYR_I2C_PORT_H_INCLUDED
#define ZEPHYR_I2C_PORT_H_INCLUDED


#include <stdint.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>


int  zephyr_i2c_port_transfer(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf, size_t readlen,
                              void *arg);
void zephyr_i2c_port_scan(const struct device *i2c_dev);


#endif
