#ifndef POSIX_I2C_PORT_H_INCLUDED
#define POSIX_I2C_PORT_H_INCLUDED

#include "../../i2c_common/i2c_common.h"


int posix_eeprom_file_i2c_u8addr_port_transfer(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf,
                                               size_t readlen, void *arg);

#endif