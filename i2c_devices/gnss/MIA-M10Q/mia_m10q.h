#ifndef MIA_M10Q_H_INCLUDED
#define MIA_M10Q_H_INCLUDED


#include "../../../i2c_common/i2c_common.h"


#define MIA_M10Q_DEFAULT_ADDRESS 0x84

#define MIA_M10Q_CFG_I2COUTPROT_UBX_ID      0x10720001
#define MIA_M10Q_CFG_I2COUTPROT_NMEA_ID     0x10720002
#define MIA_M10Q_CFG_MSGOUT_UBX_NAV_PVT_I2C 0x20910006

#define MIA_M10Q_VALUE(KeyId, Value)                                                                                   \
    _Generic((Value), uint8_t                                                                                          \
             : (struct mia_m10q_value){.key_id = (KeyId), .size = MIA_M10Q_VALUE_SIZE_U1, .as.u1 = (Value)}, default   \
             : (struct mia_m10q_value){.key_id = (KeyId), .size = MIA_M10Q_VALUE_SIZE_U1, .as.u1 = (Value)})



enum mia_m10q_response {
    MIA_M10Q_RESPONSE_OK = 0,
    MIA_M10Q_RESPONSE_INCOMPLETE,
    MIA_M10Q_RESPONSE_MISSING_HEADER,
    MIA_M10Q_RESPONSE_INVALID_CRC,
    MIA_M10Q_RESPONSE_NACK,
    MIA_M10Q_RESPONSE_ERROR,
    MIA_M10Q_RESPONSE_I2C_ERROR,
    MIA_M10Q_RESPONSE_OOM,
};


enum mia_m10q_value_size {
    MIA_M10Q_VALUE_SIZE_L  = 0,
    MIA_M10Q_VALUE_SIZE_U1 = 0,
};


struct mia_m10q_value {
    uint32_t                 key_id;
    enum mia_m10q_value_size size;
    union {
        uint8_t u1;
    } as;
};


struct mia_m10q_pvt_data {
    int32_t longitude;
    int32_t latitude;
    uint8_t fix_type;
    uint8_t flags;
};


struct mia_m10q_versions {
    char sw_version[30];
    char hw_version[10];
};


enum mia_m10q_response mia_m10q_get_available_bytes(i2c_driver_t driver, uint16_t available_bytes[static 1]);
int mia_m10q_receive_ubx_message(i2c_driver_t driver, size_t max_length, uint8_t buffer[static max_length],
                                 uint32_t timeout_ms);
int mia_m10q_receive_char_message(i2c_driver_t driver, size_t max_length, char buffer[static max_length]);
enum mia_m10q_response mia_m10q_receive_ubx_message_alloc(i2c_driver_t driver, uint8_t **buffer, uint32_t timeout_ms);
int                    mia_m10q_receive_char_message_alloc(i2c_driver_t driver, char **buffer);

enum mia_m10q_response mia_m10q_cfg_valget(i2c_driver_t driver, struct mia_m10q_value *value);
enum mia_m10q_response mia_m10q_cfg_valset(i2c_driver_t driver, struct mia_m10q_value value);
enum mia_m10q_response mia_m10q_get_position(i2c_driver_t driver, struct mia_m10q_pvt_data *pvt_data);
enum mia_m10q_response mia_m10q_get_position_velocity_time(i2c_driver_t driver, struct mia_m10q_pvt_data *pvt_data);
enum mia_m10q_response mia_m10q_get_versions(i2c_driver_t driver, struct mia_m10q_versions *versions);
int                    mia_m10q_flush(i2c_driver_t driver);


#endif
