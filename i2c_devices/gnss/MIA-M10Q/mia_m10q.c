#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "mia_m10q.h"

#include <stdio.h>


#define DATA_AVAILABLE_BYTES_HIGH 0xFD
#define DATA_STREAM_ADDRESS       0xFF
#define EOF_VALUE                 0xFF
#define HEADER_1                  0xB5
#define HEADER_2                  0x62
#define MIN_MESSAGE_LENGTH        8

#define ID_UBX_NACK        0x00
#define ID_UBX_ACK         0x01
#define ID_UBX_NAV_POS_LLH 0x02
#define ID_UBX_MON_VER     0x04
#define ID_UBX_NAV_PVT     0x07
#define ID_UBX_NAV_SAT     0x35
#define ID_UBX_CFG_VALSET  0x8A
#define ID_UBX_CFG_VALGET  0x8B

#define TIMEOUT_MS 10


static uint16_t crc(size_t len, uint8_t buffer[len]);
static int      serialize_ubx_message(size_t buffer_len, uint8_t buffer[buffer_len], uint8_t class, uint8_t id,
                                      size_t payload_len, uint8_t payload[]);
static enum mia_m10q_response is_ubx_message_valid(size_t buffer_len, uint8_t buffer[buffer_len]);
static int                    skip_until_header(i2c_driver_t driver);
static enum mia_m10q_response validate_ack_response(i2c_driver_t driver, uint8_t class, uint8_t id,
                                                    uint32_t timeout_ms);


enum mia_m10q_response mia_m10q_get_available_bytes(i2c_driver_t driver, uint16_t available_bytes[static 1]) {
    uint8_t buffer[2] = {};
    int     res       = i2c_read_register(driver, DATA_AVAILABLE_BYTES_HIGH, buffer, 2);

    *available_bytes = ((uint16_t)buffer[0] << 8) | ((uint16_t)buffer[1]);

    return res < 0 ? MIA_M10Q_RESPONSE_I2C_ERROR : MIA_M10Q_RESPONSE_OK;
}


int mia_m10q_send_message(i2c_driver_t driver, size_t len, uint8_t message[len]) {
    return i2c_write_register(driver, DATA_STREAM_ADDRESS, message, len);
}


enum mia_m10q_response mia_m10q_cfg_valget(i2c_driver_t driver, struct mia_m10q_value *value) {
    uint8_t payload[15]                                   = {};
    uint8_t message[MIN_MESSAGE_LENGTH + sizeof(payload)] = {};

    size_t i = 0;

    payload[i++] = 0x00;
    payload[i++] = 0x00;     // Read from RAM only
    payload[i++] = 0x00;     // Position
    payload[i++] = 0x00;     // ^

    payload[i++] = value->key_id & 0xFF;
    payload[i++] = (value->key_id >> 8) & 0xFF;
    payload[i++] = (value->key_id >> 16) & 0xFF;
    payload[i++] = (value->key_id >> 24) & 0xFF;

    int len = serialize_ubx_message(sizeof(message), message, 0x6, ID_UBX_CFG_VALGET, i, payload);

    int res = mia_m10q_send_message(driver, len, message);
    if (res < 0) {
        return MIA_M10Q_RESPONSE_I2C_ERROR;
    }

    uint8_t response[MIN_MESSAGE_LENGTH + 4 + 12] = {};
    res = mia_m10q_receive_ubx_message(driver, sizeof(response), response, TIMEOUT_MS);
    if (res < MIN_MESSAGE_LENGTH) {
        return MIA_M10Q_RESPONSE_INCOMPLETE;
    }

    enum mia_m10q_response response_code = is_ubx_message_valid(res, response);
    if (response_code != MIA_M10Q_RESPONSE_OK) {
        return response_code;
    }

    switch (value->size) {
        case MIA_M10Q_VALUE_SIZE_U1: {
            value->as.u1 = response[14];
            break;
        }

        default:
            assert(0);
    }

    if (!(response[2] == 0x6 && response[3] == ID_UBX_CFG_VALGET && response[6] == 1)) {
        return MIA_M10Q_RESPONSE_ERROR;
    }

    return validate_ack_response(driver, 0x6, ID_UBX_CFG_VALGET, TIMEOUT_MS);
}


enum mia_m10q_response mia_m10q_cfg_valset(i2c_driver_t driver, struct mia_m10q_value value) {
    uint8_t payload[15]                                   = {};
    uint8_t message[MIN_MESSAGE_LENGTH + sizeof(payload)] = {};

    size_t i = 0;

    payload[i++] = 0x00;
    payload[i++] = 0x01;     // Write to RAM only
    i += 2;                  // Reserved field

    payload[i++] = value.key_id & 0xFF;
    payload[i++] = (value.key_id >> 8) & 0xFF;
    payload[i++] = (value.key_id >> 16) & 0xFF;
    payload[i++] = (value.key_id >> 24) & 0xFF;

    switch (value.size) {
        case MIA_M10Q_VALUE_SIZE_U1:
            payload[i++] = value.as.u1;
            break;

        default:
            assert(0);
            break;
    }

    int len = serialize_ubx_message(sizeof(message), message, 0x6, ID_UBX_CFG_VALSET, i, payload);

    int res = mia_m10q_send_message(driver, len, message);
    if (res < 0) {
        return MIA_M10Q_RESPONSE_I2C_ERROR;
    }

    return validate_ack_response(driver, 0x6, ID_UBX_CFG_VALSET, TIMEOUT_MS);
}


enum mia_m10q_response mia_m10q_get_versions(i2c_driver_t driver, struct mia_m10q_versions *versions) {
    uint8_t message[MIN_MESSAGE_LENGTH] = {};
    int     len                         = serialize_ubx_message(sizeof(message), message, 0xa, ID_UBX_MON_VER, 0, NULL);
    assert(len > 0);

    int res = mia_m10q_send_message(driver, len, message);
    if (res < 0) {
        return MIA_M10Q_RESPONSE_I2C_ERROR;
    }

    uint8_t *response = NULL;
    res               = mia_m10q_receive_ubx_message_alloc(driver, &response, TIMEOUT_MS);
    if (res != MIA_M10Q_RESPONSE_OK) {
        return res;
    }

    if (!is_ubx_message_valid(res, response)) {
        free(response);
        return -1;
    }

    memcpy(versions->sw_version, &response[MIN_MESSAGE_LENGTH - 2], sizeof(versions->sw_version));
    memcpy(versions->hw_version, &response[MIN_MESSAGE_LENGTH - 2 + 30], sizeof(versions->hw_version));

    free(response);
    return 0;
}


enum mia_m10q_response mia_m10q_get_message(i2c_driver_t driver, struct mia_m10q_message *message) {
    uint8_t response[MIN_MESSAGE_LENGTH + 1024] = {};

    int length = mia_m10q_receive_ubx_message(driver, sizeof(response), response, 0);

    if (length > 0) {
        int res = is_ubx_message_valid(length, response);

        if (res != MIA_M10Q_RESPONSE_OK) {
            return res;
        } else if (response[2] == 1) {
            switch (response[3]) {
                case ID_UBX_NAV_PVT: {
                    message->tag                  = MIA_M10Q_MESSAGE_TAG_NAV_PVT;
                    message->as.nav_pvt.longitude = (response[MIN_MESSAGE_LENGTH - 2 + 24] & 0xFF) |
                                                    ((response[MIN_MESSAGE_LENGTH - 2 + 25] & 0xFF) << 8) |
                                                    ((response[MIN_MESSAGE_LENGTH - 2 + 26] & 0xFF) << 16) |
                                                    ((response[MIN_MESSAGE_LENGTH - 2 + 27] & 0xFF) << 24);
                    message->as.nav_pvt.latitude = (response[MIN_MESSAGE_LENGTH - 2 + 28] & 0xFF) |
                                                   ((response[MIN_MESSAGE_LENGTH - 2 + 29] & 0xFF) << 8) |
                                                   ((response[MIN_MESSAGE_LENGTH - 2 + 30] & 0xFF) << 16) |
                                                   ((response[MIN_MESSAGE_LENGTH - 2 + 31] & 0xFF) << 24);
                    message->as.nav_pvt.fix_type = response[MIN_MESSAGE_LENGTH - 2 + 20];
                    message->as.nav_pvt.flags    = response[MIN_MESSAGE_LENGTH - 2 + 21];
                    break;
                }

                case ID_UBX_NAV_SAT: {
                    message->tag                = MIA_M10Q_MESSAGE_TAG_NAV_SAT;
                    message->as.nav_sat.num_svs = response[MIN_MESSAGE_LENGTH - 2 + 5];

                    uint16_t total = 0;
                    uint16_t count = 0;

                    for (size_t i = 0; i < message->as.nav_sat.num_svs && (MIN_MESSAGE_LENGTH + 8 + i * 12) < length;
                         i++) {
                        size_t  base_index = MIN_MESSAGE_LENGTH - 2 + 8 + i * 12;
                        uint8_t flags      = response[base_index + 8];

                        // Signal is used
                        if (flags & (1 << 3)) {
                            total += response[base_index + 2];
                            count++;
                        }
                    }

                    if (count > 0) {
                        message->as.nav_sat.average_cno = total / count;
                    } else {
                        message->as.nav_sat.average_cno = 0;
                    }
                    break;
                }
            }

            return MIA_M10Q_RESPONSE_OK;
        } else {
            return MIA_M10Q_RESPONSE_ERROR;
        }
    } else {
        return MIA_M10Q_RESPONSE_INCOMPLETE;
    }
}


enum mia_m10q_response mia_m10q_get_position_velocity_time(i2c_driver_t driver, struct mia_m10q_pvt_data *pvt_data) {
    int res = 0;

    uint8_t response[MIN_MESSAGE_LENGTH + 92] = {};

    res = mia_m10q_receive_ubx_message(driver, sizeof(response), response, 0);

    if (res < sizeof(response)) {
        return MIA_M10Q_RESPONSE_INCOMPLETE;
    }

    res = is_ubx_message_valid(sizeof(response), response);
    if (res != MIA_M10Q_RESPONSE_OK) {
        return res;
    }

    pvt_data->longitude =
        (response[MIN_MESSAGE_LENGTH - 2 + 24] & 0xFF) | ((response[MIN_MESSAGE_LENGTH - 2 + 25] & 0xFF) << 8) |
        ((response[MIN_MESSAGE_LENGTH - 2 + 26] & 0xFF) << 16) | ((response[MIN_MESSAGE_LENGTH - 2 + 27] & 0xFF) << 24);
    pvt_data->latitude =
        (response[MIN_MESSAGE_LENGTH - 2 + 28] & 0xFF) | ((response[MIN_MESSAGE_LENGTH - 2 + 29] & 0xFF) << 8) |
        ((response[MIN_MESSAGE_LENGTH - 2 + 30] & 0xFF) << 16) | ((response[MIN_MESSAGE_LENGTH - 2 + 31] & 0xFF) << 24);
    pvt_data->fix_type = response[MIN_MESSAGE_LENGTH - 2 + 20];
    pvt_data->flags    = response[MIN_MESSAGE_LENGTH - 2 + 21];

    return MIA_M10Q_RESPONSE_OK;
}


enum mia_m10q_response mia_m10q_get_position(i2c_driver_t driver, struct mia_m10q_pvt_data *pvt_data) {
    uint8_t message[MIN_MESSAGE_LENGTH] = {};
    int     len = serialize_ubx_message(sizeof(message), message, 1, ID_UBX_NAV_POS_LLH, 0, NULL);
    assert(len > 0);

    int res = mia_m10q_send_message(driver, len, message);
    if (res < 0) {
        return MIA_M10Q_RESPONSE_I2C_ERROR;
    }

    driver.delay_ms(100);

    uint8_t response[MIN_MESSAGE_LENGTH + 28] = {};

    res = mia_m10q_receive_ubx_message(driver, sizeof(response), response, 10);

    if (res < sizeof(response)) {
        return MIA_M10Q_RESPONSE_INCOMPLETE;
    }

    res = is_ubx_message_valid(sizeof(response), response);
    if (res != MIA_M10Q_RESPONSE_OK) {
        return res;
    }

    pvt_data->longitude =
        (response[MIN_MESSAGE_LENGTH - 2 + 4] & 0xFF) | ((response[MIN_MESSAGE_LENGTH - 2 + 5] & 0xFF) << 8) |
        ((response[MIN_MESSAGE_LENGTH - 2 + 6] & 0xFF) << 16) | ((response[MIN_MESSAGE_LENGTH - 2 + 7] & 0xFF) << 24);
    pvt_data->latitude =
        (response[MIN_MESSAGE_LENGTH - 2 + 8] & 0xFF) | ((response[MIN_MESSAGE_LENGTH - 2 + 9] & 0xFF) << 8) |
        ((response[MIN_MESSAGE_LENGTH - 2 + 10] & 0xFF) << 16) | ((response[MIN_MESSAGE_LENGTH - 2 + 11] & 0xFF) << 24);

    /*
    pvt_data->longitude =
        (response[MIN_MESSAGE_LENGTH - 2 + 24] & 0xFF) | ((response[MIN_MESSAGE_LENGTH - 2 + 25] & 0xFF) << 8) |
        ((response[MIN_MESSAGE_LENGTH - 2 + 26] & 0xFF) << 16) | ((response[MIN_MESSAGE_LENGTH - 2 + 27] & 0xFF) << 24);
    pvt_data->latitude =
        (response[MIN_MESSAGE_LENGTH - 2 + 28] & 0xFF) | ((response[MIN_MESSAGE_LENGTH - 2 + 29] & 0xFF) << 8) |
        ((response[MIN_MESSAGE_LENGTH - 2 + 30] & 0xFF) << 16) | ((response[MIN_MESSAGE_LENGTH - 2 + 31] & 0xFF) << 24);
    pvt_data->fix_type = response[MIN_MESSAGE_LENGTH - 2 + 20];
    pvt_data->flags    = response[MIN_MESSAGE_LENGTH - 2 + 21];
    */

    return MIA_M10Q_RESPONSE_OK;
}


int mia_m10q_flush(i2c_driver_t driver, unsigned bytes) {
    uint8_t command = DATA_STREAM_ADDRESS;
    int     res     = driver.i2c_transfer(driver.device_address, &command, 1, NULL, 0, driver.arg);
    if (res < 0) {
        return res;
    }

    size_t  bytes_read = 0;
    uint8_t byte       = 0;

    while (res == 0 && ((bytes > 0 && bytes_read < bytes) || byte != EOF_VALUE)) {
        res = driver.i2c_transfer(driver.device_address, NULL, 0, &byte, 1, driver.arg);
        if (res < 0) {
            return res;
        }
        bytes_read++;
    }

    return bytes_read;
}


int mia_m10q_receive_char_message_alloc(i2c_driver_t driver, char **return_buffer) {
    uint8_t command = DATA_STREAM_ADDRESS;
    int     res     = driver.i2c_transfer(driver.device_address, &command, 1, NULL, 0, driver.arg);
    if (res < 0) {
        return res;
    }

    size_t  bytes_read = 0;
    uint8_t byte       = 0;

    size_t allocated_size = 128;
    char  *buffer         = malloc(allocated_size);
    if (!buffer) {
        return -1;
    }

    while (res == 0 && byte != EOF_VALUE) {
        res = driver.i2c_transfer(driver.device_address, NULL, 0, &byte, 1, driver.arg);
        if (res < 0) {
            return res;
        }

        if (byte != EOF_VALUE) {
            buffer[bytes_read] = byte;
            bytes_read++;
        }

        if (bytes_read >= allocated_size - 1) {
            allocated_size *= 2;
            uint8_t *new_buffer = realloc(buffer, allocated_size);
            if (!new_buffer) {
                free(buffer);
                return -1;
            }

            buffer = new_buffer;
        }
    }

    buffer[bytes_read] = '\0';

    *return_buffer = buffer;

    return bytes_read;
}


enum mia_m10q_response mia_m10q_receive_ubx_message_alloc(i2c_driver_t driver, uint8_t **return_buffer,
                                                          uint32_t timeout_ms) {
    uint8_t command = DATA_STREAM_ADDRESS;
    int     res     = driver.i2c_transfer(driver.device_address, &command, 1, NULL, 0, driver.arg);
    if (res < 0) {
        return MIA_M10Q_RESPONSE_I2C_ERROR;
    }

    size_t  bytes_read = 0;
    uint8_t byte       = 0;

    size_t   allocated_size = MIN_MESSAGE_LENGTH;
    uint8_t *buffer         = malloc(allocated_size);
    if (!buffer) {
        return MIA_M10Q_RESPONSE_OOM;
    }

    bool     payload_len_parsed = false;
    uint16_t delayed            = 0;
    enum {
        HEADER_STATE_NONE,
        HEADER_STATE_BEGUN,
        HEADER_STATE_RECEIVED,
    } header_state = HEADER_STATE_NONE;

    while (res == 0 && bytes_read < allocated_size && (byte != EOF_VALUE || delayed < timeout_ms)) {
        res = driver.i2c_transfer(driver.device_address, NULL, 0, &byte, 1, driver.arg);
        if (res < 0) {
            return MIA_M10Q_RESPONSE_I2C_ERROR;
        }

        if (byte != EOF_VALUE) {
            switch (header_state) {
                case HEADER_STATE_NONE: {
                    // printf("(0x%02X) ", byte);
                    if (byte == HEADER_1) {
                        header_state = HEADER_STATE_BEGUN;
                    }
                    break;
                }
                case HEADER_STATE_BEGUN: {
                    // printf("(0x%02X) ", byte);
                    if (byte == HEADER_2) {
                        bytes_read           = 0;
                        buffer[bytes_read++] = HEADER_1;
                        buffer[bytes_read++] = HEADER_2;
                        header_state         = HEADER_STATE_RECEIVED;
                    } else {
                        header_state = HEADER_STATE_NONE;
                    }
                    break;
                }
                case HEADER_STATE_RECEIVED: {
                    // printf("0x%02X ", byte);
                    buffer[bytes_read++] = byte;
                    break;
                }
            }
        } else {
            driver.delay_ms(1);
            delayed++;
        }

        if (!payload_len_parsed && bytes_read >= allocated_size) {
            uint16_t payload_len = ((uint16_t)buffer[5] << 8) | ((uint16_t)buffer[4]);

            allocated_size += payload_len;
            uint8_t *new_buffer = realloc(buffer, allocated_size);
            // printf("Reallocating %zu (%p)\n", allocated_size, new_buffer);
            if (!new_buffer) {
                free(buffer);
                return MIA_M10Q_RESPONSE_OOM;
            }

            buffer             = new_buffer;
            payload_len_parsed = true;
        }
    }
    // printf("\n");

    *return_buffer = buffer;

    return MIA_M10Q_RESPONSE_OK;
}


int mia_m10q_receive_char_message(i2c_driver_t driver, size_t max_length, char buffer[static max_length]) {
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

    while (res == 0 && bytes_read + 1 < max_length && byte != EOF_VALUE) {
        res = driver.i2c_transfer(driver.device_address, NULL, 0, &byte, 1, driver.arg);
        if (res < 0) {
            return res;
        }

        if (byte != EOF_VALUE) {
            buffer[bytes_read++] = byte;
        }
    }

    buffer[bytes_read] = '\0';

    return bytes_read;
}



int mia_m10q_receive_ubx_message(i2c_driver_t driver, size_t max_length, uint8_t buffer[static max_length],
                                 uint32_t timeout_ms) {
    if (max_length == 0) {
        return -1;
    }

    uint8_t command = DATA_STREAM_ADDRESS;
    int     res     = driver.i2c_transfer(driver.device_address, &command, 1, NULL, 0, driver.arg);
    if (res < 0) {
        return res;
    }

    size_t   bytes_read         = 0;
    uint8_t  byte               = 0;
    bool     payload_len_parsed = false;
    uint16_t payload_len        = 0;
    uint16_t delayed            = 0;
    enum {
        HEADER_STATE_NONE,
        HEADER_STATE_BEGUN,
        HEADER_STATE_RECEIVED,
    } header_state = HEADER_STATE_NONE;

    // printf("Receiving ");
    while (res == 0 && bytes_read < max_length && (bytes_read < MIN_MESSAGE_LENGTH + payload_len) &&
           delayed <= timeout_ms) {
        res = driver.i2c_transfer(driver.device_address, NULL, 0, &byte, 1, driver.arg);
        if (res < 0) {
            return res;
        }

        switch (header_state) {
            case HEADER_STATE_NONE: {
                // printf("(0x%02X) ", byte);
                if (byte == EOF_VALUE) {
                    driver.delay_ms(1);
                    delayed++;
                } else if (byte == HEADER_1) {
                    header_state = HEADER_STATE_BEGUN;
                }
                break;
            }
            case HEADER_STATE_BEGUN: {
                // printf("(0x%02X) ", byte);
                if (byte == EOF_VALUE) {
                    driver.delay_ms(1);
                    delayed++;
                } else if (byte == HEADER_2) {
                    bytes_read           = 0;
                    buffer[bytes_read++] = HEADER_1;
                    buffer[bytes_read++] = HEADER_2;
                    header_state         = HEADER_STATE_RECEIVED;
                } else {
                    header_state = HEADER_STATE_NONE;
                }
                break;
            }
            case HEADER_STATE_RECEIVED: {
                // printf("0x%02X ", byte);
                buffer[bytes_read++] = byte;
                break;
            }
        }

        if (!payload_len_parsed && bytes_read >= MIN_MESSAGE_LENGTH) {
            payload_len        = ((uint16_t)buffer[5] << 8) | ((uint16_t)buffer[4]);
            payload_len_parsed = true;
        }
    }
    // printf("\n");

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


static int skip_until_header(i2c_driver_t driver) {
    uint8_t byte = 0;
    int     res  = 0;

    uint16_t header = 0;

    while (res == 0 && byte != EOF_VALUE && header < 2) {
        res = driver.i2c_transfer(driver.device_address, NULL, 0, &byte, 1, driver.arg);
        if (res < 0) {
            return res;
        }

        if (header == 0 && byte == HEADER_1) {
            header++;
        } else if (header == 1 && byte == HEADER_2) {
            header++;
        } else {
            header = 0;
        }
    }

    return header;
}



static enum mia_m10q_response is_ubx_message_valid(size_t buffer_len, uint8_t buffer[buffer_len]) {
    if (buffer_len < MIN_MESSAGE_LENGTH) {
        return MIA_M10Q_RESPONSE_INCOMPLETE;
    } else if (buffer[0] != HEADER_1 || buffer[1] != HEADER_2) {
        return MIA_M10Q_RESPONSE_MISSING_HEADER;
    } else {
        uint16_t crc_calculated = crc(buffer_len - 4, &buffer[2]);

        uint16_t payload_len = ((uint16_t)buffer[5] << 8) | ((uint16_t)buffer[4]);
        if (payload_len > buffer_len - MIN_MESSAGE_LENGTH) {
            return MIA_M10Q_RESPONSE_INCOMPLETE;
        }

        uint16_t crc_read = ((uint16_t)buffer[MIN_MESSAGE_LENGTH - 2 + payload_len] << 8) |
                            ((uint16_t)buffer[MIN_MESSAGE_LENGTH - 1 + payload_len]);

        return crc_calculated == crc_read ? MIA_M10Q_RESPONSE_OK : MIA_M10Q_RESPONSE_INVALID_CRC;
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


static enum mia_m10q_response validate_ack_response(i2c_driver_t driver, uint8_t class, uint8_t id,
                                                    uint32_t timeout_ms) {
    uint8_t response[MIN_MESSAGE_LENGTH + 2] = {};
    int     res = mia_m10q_receive_ubx_message(driver, sizeof(response), response, timeout_ms);
    if (res < sizeof(response)) {
        return MIA_M10Q_RESPONSE_INCOMPLETE;
    }

    enum mia_m10q_response response_code = is_ubx_message_valid(res, response);
    if (response_code != MIA_M10Q_RESPONSE_OK) {
        return response_code;
    }

    if (response[2] == 0x5 && response[3] == ID_UBX_ACK && response[6] == class && response[7] == id) {
        return MIA_M10Q_RESPONSE_OK;
    } else if (response[2] == 0x5 && response[3] == ID_UBX_NACK && response[6] == class && response[7] == id) {
        return MIA_M10Q_RESPONSE_NACK;
    } else {
        return MIA_M10Q_RESPONSE_ERROR;
    }
}
