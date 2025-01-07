# I2C Peripheral library

A collection of I2C drivers for various integrated circuits. 

The idea is to abstract away the hardware details ([the data link layer](https://en.wikipedia.org/wiki/Data_link_layer)) and create drivers that implement the API required by each device.

Nothing here is particularly complex, but it is aggregated and standardized for ease of use over multiple and diverse projects.

Similar libraries for other protocols:
- [c-spi-drivers](https://github.com/Maldus512/c-spi-drivers)

# How it works

The hardware is abstracted through the `i2c_driver_t` structure and its 5 fields:

 - `device_address`: I2C address for the required IC. The address should be human readable as in 8 bits long, 
    the first being changed for read and write operations according to the I2C protocol.
 - `i2c_transfer`: pointer to a function that sends some data on the I2C network and then reads the answer. It is hardware
    dependant and should be provided by the user.
 - `ack_polling`: a peculiar function used only on some EEPROM ICs. It should continuously poll the device, waiting for a response. Not strictly necessary.
 - `delay_ms`: pointer to a function that blocks for the required time in milliseconds.
 - `arg`: `void*` user argument that is passed to each function pointer.

## Example

The following example defines a FreeRTOS task that reads temperature and humidity values every second.
The hardware abstraction layer should be already initialized and ready.

```
#include "i2c_devices.h"
#include "i2c_common/i2c_common.h"
#include "i2c_ports/esp-idf/esp_idf_i2c_port.h"
#include "i2c_devices/temperature/SHTC3/shtc3.h"


static void delay_ms(unsigned long ms);


i2c_driver_t shtc3_driver = {
    .device_address = SHTC3_DEFAULT_ADDRESS,
    .i2c_transfer   = esp_idf_i2c_port_transfer,
    .delay_ms       = delay_ms,
};


// Periodically read a temperature value
static void temperature_task(void *args) {
    (void)args;
    shtc3_wakeup(shtc3_driver); //Wake up the device

    for (;;) {
        int16_t temperature = 0;
        int16_t humidity    = 0;

        if (shtc3_start_temperature_humidity_measurement(shtc3_driver) == 0) {
            vTaskDelay(pdMS_TO_TICKS(SHTC3_NORMAL_MEASUREMENT_PERIOD_MS));

            if (shtc3_read_temperature_humidity_measurement(shtc3_driver, &temperature, &humidity) == 0) {
                printf("Sensor values: %iC %i%%", temperature, humidity);
            } else {
                printf("Error in reading temperature measurement!\n");
            }
        } else {
            printf("Error in starting temperature measurement\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}


static void delay_ms(unsigned long ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}
```

# Project Structure

There are three main folders:
 - `i2c_common`: module for functions and data structures common to all device drivers. Contains the typedef `i2c_driver_t`.
 - `i2c_devices`: folder with all device drivers
 - `i2c_ports`: optional hardware abstraction layer implementations for some common architectures.

## Device drivers

Each directory under `i2c_devices` contains a driver for a different device. Currently supported devices are:

 - `accelerometer`: Accelerometer devices
    - `WSEN_ITDS`: [Wurth 3-Axis accelerometer](https://www.we-online.com/components/products/manual/2533020201601_WSEN-ITDS%202533020201601%20Manual_rev2.3.pdf)

 - `dac`: Digital to analog converters
    - `MCP4018`: [Microchip digital potentiometer](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/22147a.pdf)

 - `eeprom`: EEPROM memories
    - `24LC16`: [Microchip 1Kbit EEPROM memory](https://ww1.microchip.com/downloads/en/DeviceDoc/20002213B.pdf)
    - `24AA32`: [Microchip 32Kbi EEPROM memory](https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/24AA32A-24LC32A-32-Kbit-I2C-Serial-EEPROM-20001713N.pdf)
    - `24LC1025`: [Microchip 1024Kbit EEPROM memory](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/20001941L.pdf)

 - `io`: Port expanders
    - `MCP23008`: [Microchip 8-bit port expander (i2c version)](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf)
    - `TCA9534`: [Texas Instruments 8-bit port expander](https://www.ti.com/lit/ds/symlink/tca9534.pdf?ts=1689164274927&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FTCA9534%253Futm_source%253Dgoogle%2526utm_medium%253Dcpc%2526utm_campaign%253Dasc-null-null-gpn_en-cpc-pf-google-wwe%2526utm_content%253Dtca9534%2526ds_k%253DTCA9534%2526dcm%253Dyes%2526gclid%253DEAIaIQobChMI6ejvn5OJgAMV5z0GAB0qwwaFEAAYASAAEgKtt_D_BwE%2526gclsrc%253Daw.ds)

 - `light_proximity`: Light and proximity sensors
    - `LTR559ALS`: [Liteon optical sensor](https://optoelectronics.liteon.com/upload/download/ds86-2013-0003/ltr-559als-01_ds_v1.pdf)
    - `VCNL4010`: [Vishay proximity and light sensor](https://www.vishay.com/docs/83462/vcnl4010.pdf)

 - `rtc`: Real Time Clocks
    - `DS1307`: [Analog RTC](https://www.analog.com/media/en/technical-documentation/data-sheets/DS1307.pdf)
    - `M41T81`: [STM RTC](https://www.st.com/en/clocks-and-timers/m41t81.html)
    - `PCF85063A`: [NXP RTC](https://www.nxp.com/docs/en/data-sheet/PCF85063A.pdf)
    - `PCF8523`: [NXP RTC](https://www.nxp.com/docs/en/data-sheet/PCF8523A.pdf)
    - `RX8010`: [Epson RTC](https://www5.epsondevice.com/en/products/rtc/rx8010sj.html)

 - `temperature`: Temperature, humidity and pressure sensors
    - `MCP9800`: [Microchip temperature sensor](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/21909d.pdf)
    - `MS5837`: [TE temperature and pressure sensor](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS5837-30BA%7FB1%7Fpdf%7FEnglish%7FENG_DS_MS5837-30BA_B1.pdf%7FCAT-BLPS0017)
    - `SHT21` : [Sensirion temperature and humidity sensor](https://www.mouser.it/datasheet/2/682/Sensirion_Datasheet_Humidity_Sensor_SHT21-3051666.pdf)
    - `SHT3` : [Sensirion temperature and humidity sensor](https://www.mouser.com/datasheet/2/682/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital-971521.pdf)
    - `SHT4` : [Sensirion temperature and humidity sensor](https://developer.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHT4x_Datasheet.pdf)
    - `SHTC3` : [Sensirion temperature and humidity sensor](https://www.mouser.it/datasheet/2/682/seri_s_a0003561073_1-2291167.pdf)
    - `TC74` : [Microchip temperature sensor](https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/21462D.pdf)
    - `ZS05` : [Winsen temperature and humidity sensor](https://www.winsen-sensor.com/d/files/zs05-temperature-and-humidity-module-manual-v1_8.pdf)

# Porting

As mentioned, this library is hardware agnostic and requires an hardware abstraction layer in the form of the `i2c_transfer` callback to work.
In practice this means instantiating the `i2c_driver_t` structure.

```
const i2c_driver_t sht31_driver =
{ .device_address = 0x88, .i2c_transfer = esp_idf_i2c_port_transfer };
```

`device_address` and `i2c_transfer` are the only two mandatory fields (although some drivers, in some cases, also require the `delay_ms` callback).
The device address is then passed to `i2c_transfer` invokations during the driver's work.

`i2c_transfer` should point to a function with the following signature:

```
int (*i2c_transfer)(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf, size_t readlen, void *arg);
```

 - `devaddr` is the device address as specified inside the structure
 - `writebuf` is the buffer containing data that should be written on the I2C network. Can be `NULL` if no write is required
 - `writelen` is the number of bytes to write (taken from `writebuf`)
 - `readbuf` is the buffer where data will be transferred from the I2C network after the write operation. Can be `NULL` if no read is required
 - `readlen` is the number of bytes to read (transferred in `readbuf`)
 - `arg` is the user pointer specified in the driver structure

Then, every function that makes up the corresponding device driver will take the `i2c_driver_t` instance as first parameter.

The `i2c_ports` folder contains some notable ports:
 
  - `dummy`: a simple port with an HAL that does nothing. Useful for stubs, tests and simulations.
  - `esp-idf`: port to the ESP-IDF framework.
  - `PIC`: ragged bitbang driver for PIC microcontrollers. I've found that often times the processor internal modules
            for networks like SPI and I2C simply don't work as advertised, and even when they do they are simply unusable.
            Bitbang routines are less efficient but cause no trouble.
  - `posix`: port that emulates EEPROMs through posix files.

# Documentation

As of now only fairly simple devices are included; documention should be studied from the comments in the header files.

# Building

## SCons

This library uses `SCons` as a build tool. To include it in your project just refer to the `SConscript` and expose two variables:
`i2c_env` should contain the compilation environment and `i2c_selected` should be a string list with the modules that should be included.

Invoking the `SConscript` returns a tuple containing the compiled library as a static archive and a list of directories that should 
be added to the include path.

```
    i2c_env = env
    i2c_selected = ['temperature/SHT3',
                    'temperature/SHT21', 'LTR559ALS', 'dummy']
    (i2c_lib, include) = SConscript(
        f'{I2C}/SConscript', exports=['i2c_env', 'i2c_selected'])
    env['CPPPATH'] += [include]
    prog = env.Program(PROGRAM, sources + i2c_lib)
```

## ESP-IDF

Aside from `SCons` this is also a ready-to-use ESP-IDF component. Just add this repository as a submodule in the `components` folder of your project
and configure the required module through the `KConfig` interface. 

## Other Platforms

Each device driver is composed by very few source files with no dependencies beside `i2c_common`; porting any of those to another build system should be trivial.

# TODO

- Raise all code level up to standard (e.g. PIC ports)
- Add a generic bitbang port
