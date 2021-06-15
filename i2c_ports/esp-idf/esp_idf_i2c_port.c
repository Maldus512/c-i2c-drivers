#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "../../i2c_common/i2c_common.h"

static const char *TAG = "I2C PORT";


void esp_idf_i2c_scanner(void) {
    ESP_LOGI(TAG, ">> i2cScanner");
    int       i;
    esp_err_t espRc;
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("00:         ");
    for (i = 3; i < 0x78; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_stop(cmd);

        espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
        if (i % 16 == 0) {
            printf("\n%.2x:", i);
        }
        if (espRc == 0) {
            printf(" %.2x", i << 1);
        } else {
            printf(" --");
        }
        i2c_cmd_link_delete(cmd);
    }
    printf("\n");
}


int esp_idf_i2c_port_transfer(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf, size_t readlen,
                              void *arg) {
    esp_err_t        ret = ESP_OK;
    i2c_cmd_handle_t cmd;
    (void)arg;

    if (writebuf != NULL && writelen > 0) {
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, devaddr, 1);
        i2c_master_write(cmd, writebuf, writelen, 1);
        i2c_master_stop(cmd);

        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(10));
        i2c_cmd_link_delete(cmd);

        if (ret != ESP_OK)
            return ret;
    }

    if (readbuf != NULL && readlen > 0) {
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, devaddr | 1, 1);
        i2c_master_read(cmd, readbuf, readlen, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);

        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(10));
        i2c_cmd_link_delete(cmd);
    }

    return ret;
}


void esp_idf_i2c_ack_polling(uint8_t devaddr, void *arg) {
    (void)devaddr;
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(5));
}