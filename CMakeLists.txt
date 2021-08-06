list(APPEND MODULES "i2c_common")

if (CONFIG_I2C_DEVICES_SHT21)
    list(APPEND MODULES "i2c_devices/temperature/SHT21")
endif()
if (CONFIG_I2C_DEVICES_SHT3)
    list(APPEND MODULES "i2c_devices/temperature/SHT3")
endif()
if (CONFIG_I2C_DEVICES_RTC_PCF85063A)
    list(APPEND MODULES "i2c_devices/rtc/PCF85063A")
endif()
if (CONFIG_I2C_DEVICES_LTR559ALS)
    list(APPEND MODULES "i2c_devices/LTR559ALS")
endif()
if (CONFIG_I2C_DEVICES_MCP4018)
    list(APPEND MODULES "i2c_devices/MCP4018")
endif()
if (CONFIG_I2C_DEVICES_EEPROM_24LC16)
    list(APPEND MODULES "i2c_devices/eeprom/24LC16")
endif()
if (CONFIG_I2C_DEVICES_PIC_BITBANG_PORT)
    list(APPEND MODULES "i2c_ports/PIC")
endif()
if (CONFIG_I2C_DEVICES_ESP_IDF_PORT)
    list(APPEND MODULES "i2c_ports/esp-idf")
endif()

message(${MODULES})

idf_component_register(
    SRC_DIRS ${MODULES}
    INCLUDE_DIRS .
    )

