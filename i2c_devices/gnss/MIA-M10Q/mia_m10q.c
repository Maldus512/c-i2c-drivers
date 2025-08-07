#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "mia_m10q.h"


#define DATA_STREAM_ADDRESS 0xFF
#define EOF_VALUE           0xFF
#define HEADER_1            0xB5
#define HEADER_2            0x62
#define MIN_MESSAGE_LENGTH  8
#define ID_UBX_NAV_PVT      7


static uint16_t crc(size_t len, uint8_t buffer[len]);
static int      serialize_ubx_message(size_t buffer_len, uint8_t buffer[buffer_len], uint8_t class, uint8_t id,
                                      size_t payload_len, uint8_t payload[]);
static bool     is_ubx_message_valid(size_t buffer_len, uint8_t buffer[buffer_len]);


int mia_m10q_send_message(i2c_driver_t driver, size_t len, uint8_t message[len]) {
    return i2c_write_register(driver, DATA_STREAM_ADDRESS, message, len);
}


int mia_m10q_get_position(i2c_driver_t driver, struct mia_m10q_pvt_data *pvt_data) {
    uint8_t message[MIN_MESSAGE_LENGTH] = {};
    int     len                         = serialize_ubx_message(sizeof(message), message, 1, ID_UBX_NAV_PVT, 0, NULL);
    assert(len > 0);

    int res = mia_m10q_send_message(driver, len, message);
    if (res < 0) {
        return res;
    }

    uint8_t response[MIN_MESSAGE_LENGTH + 92] = {};
    res                                       = mia_m10q_receive_message(driver, sizeof(response), response);
    if (res < sizeof(response)) {
        return res;
    }

    if (!is_ubx_message_valid(res, response)) {
        return -1;
    }

    pvt_data->longitude =
        (response[MIN_MESSAGE_LENGTH - 2 + 24] & 0xFF) | ((response[MIN_MESSAGE_LENGTH - 2 + 25] & 0xFF) << 8) |
        ((response[MIN_MESSAGE_LENGTH - 2 + 26] & 0xFF) << 16) | ((response[MIN_MESSAGE_LENGTH - 2 + 27] & 0xFF) << 24);
    pvt_data->latitude =
        (response[MIN_MESSAGE_LENGTH - 2 + 28] & 0xFF) | ((response[MIN_MESSAGE_LENGTH - 2 + 29] & 0xFF) << 8) |
        ((response[MIN_MESSAGE_LENGTH - 2 + 30] & 0xFF) << 16) | ((response[MIN_MESSAGE_LENGTH - 2 + 31] & 0xFF) << 24);

    return 0;
}


int mia_m10q_receive_message(i2c_driver_t driver, size_t max_length, uint8_t buffer[max_length]) {
    if (max_length == 0) {
        return -1;
    }

    uint8_t command = DATA_STREAM_ADDRESS;
    int     res     = driver.i2c_transfer(driver.device_address, &command, 1, NULL, 0, driver.arg);
    if (res < 0) {
        return res;
    }

    size_t  bytes_read = 0;
    uint8_t byte       = 0;

    while (res == 0 && bytes_read < max_length && byte != EOF_VALUE) {
        res = driver.i2c_transfer(driver.device_address, NULL, 0, &byte, 1, driver.arg);
        if (res < 0) {
            return res;
        }

        if (byte != EOF_VALUE) {
            buffer[bytes_read] = byte;
            bytes_read++;
        }
    }

    return bytes_read;
}


static int serialize_ubx_message(size_t buffer_len, uint8_t buffer[buffer_len], uint8_t class, uint8_t id,
                                 size_t payload_len, uint8_t payload[]) {
    if (buffer_len < payload_len + 8) {
        return -1;
    }

    size_t i = 0;

    buffer[i++] = HEADER_1;
    buffer[i++] = HEADER_2;
    buffer[i++] = class;
    buffer[i++] = id;
    buffer[i++] = (uint8_t)(payload_len & 0xFF);
    buffer[i++] = (uint8_t)((payload_len >> 8) & 0xFF);
    if (payload != NULL) {
        memcpy(&buffer[i], payload, payload_len);
        i += payload_len;
    }

    uint16_t crc_calculated = crc(i - 2, &buffer[2]);

    buffer[i++] = (uint8_t)((crc_calculated >> 8) & 0xFF);
    buffer[i++] = (uint8_t)(crc_calculated & 0xFF);

    return (int)i;
}


static bool is_ubx_message_valid(size_t buffer_len, uint8_t buffer[buffer_len]) {
    if (buffer_len < MIN_MESSAGE_LENGTH) {
        return false;
    } else if (buffer[0] != HEADER_1 || buffer[1] != HEADER_2) {
        return false;
    } else {
        uint16_t crc_calculated = crc(buffer_len - 2, &buffer[2]);

        uint16_t payload_len = ((uint16_t)buffer[5] << 8) | ((uint16_t)buffer[4]);
        if (payload_len > buffer_len - MIN_MESSAGE_LENGTH) {
            return false;
        }

        uint16_t crc_read = ((uint16_t)buffer[MIN_MESSAGE_LENGTH - 2 + payload_len] << 8) |
                            ((uint16_t)buffer[MIN_MESSAGE_LENGTH - 1 + payload_len]);

        return crc_calculated == crc_read;
    }
}


static uint16_t crc(size_t len, uint8_t buffer[len]) {
    uint8_t ck_a = 0;
    uint8_t ck_b = 0;
    for (size_t I = 0; I < len; I++) {
        ck_a = ck_a + buffer[I];
        ck_b = ck_b + ck_a;
    }

    return (uint16_t)(ck_a << 8) | (uint16_t)ck_b;
}
