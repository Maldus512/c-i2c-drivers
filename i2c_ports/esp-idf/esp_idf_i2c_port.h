#ifndef ESP_IDF_I2C_PORT_H_INCLUDED
#define ESP_IDF_I2C_PORT_H_INCLUDED

#include "../../i2c_common/i2c_common.h"

void esp_idf_i2c_scanner(void);
int  esp_idf_i2c_port_transfer(uint8_t devaddr, uint8_t *writebuf, int writelen, uint8_t *readbuf, int readlen);
void esp_idf_i2c_ack_polling(uint8_t devaddr);

#endif