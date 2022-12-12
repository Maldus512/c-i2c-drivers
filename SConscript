import os

# TODO: Add components
components = [
    "eeprom/24LC16",
    "eeprom/24AA32",
    "temperature/SHT3",
    "temperature/SHT21",
    "temperature/SHT4",
    "rtc/PCF8523",
    "rtc/PCF85063A",
    "rtc/DS1307",
    "rtc/RX8010",
    "io/MCP23008",
    "light_proximity/LTR559ALS",
    "light_proximity/VCNL4010",
]
ports = [
    "dummy", "posix"
]
sources = {}

Import("i2c_selected", "i2c_env")

for c in components:
    sources[c] = Glob(os.path.join("./i2c_devices", c, "*.c"))

for c in ports:
    sources[c] = Glob(os.path.join("./i2c_ports", c, "*.c"))

objects = [i2c_env.Object(x) for x in Glob("./i2c_common/*.c")]

for s in i2c_selected:
    objects += i2c_env.Object(sources[s])

result = (objects, list(map(lambda x: os.path.join(
    os.getcwd(), x), [".", "i2c_ports", "i2c_devices"])))
Return("result")
