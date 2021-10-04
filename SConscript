import os

# TODO: Add components
components = [
    "eeprom/24LC16",
    "temperature/SHT3",
    "temperature/SHT21",
    "rtc/PCF8523",
    "rtc/PCF85063A",
    "LTR559ALS",
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
