#include "../../i2c_common/i2c_common.h"

int dummy_i2c_port_transfer(uint8_t devaddr, uint8_t *writebuf, int writelen, uint8_t *readbuf, int readlen) {
    return 0;
}


void dummy_i2c_ack_polling(uint8_t devaddr) {
    (void)devaddr;
}