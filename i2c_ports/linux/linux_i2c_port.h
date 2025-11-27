#ifndef I2C_PORTS_LINUX_I2C_PORT_H_INCLUDED
#define I2C_PORTS_LINUX_I2C_PORT_H_INCLUDED


#include <stdint.h>
#include <stdlib.h>


int linux_i2c_port_transfer(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf, size_t readlen,
                            void *arg);


#endif
