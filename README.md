# I2C Peripheral library

A collection of I2C drivers for various integrated circuits. The library requires a couple of callbacks to abstract the hardware layer and then implements the interface with different devices.

# How it works

The hardware is abstracted through the `i2c_device_t` structure and its 5 fields:

 - `device_address`: I2C address for the required IC. The address should be human readable as in 8 bits long, 
    the first being changed for read and write operations according to the I2C protocol.
 - `i2c_transfer`: pointer to a function that sends some data on the I2C network and then reads the answer. It is hardware
    dependant and should be provided by the user.
 - `ack_polling`: a peculiar function used only on some EEPROM ICs. It should continuously poll the device, waiting for a response. Not strictly necessary.
 - `delay_ms`: pointer to a function that blocks for the required time in milliseconds.
 - `arg`: `void*` user argument that is passed to each function pointer.

# Structure

There are three main folders:
 - `i2c_common`: module for functions and data structures common to all device drivers. Contains the typedef `i2c_device_t`.
 - `i2c_devices`: folder with all device drivers
 - `i2c_ports`: optional hardware abstraction layer implementations for some common architectures.

## Device drivers

Each directory under `i2c_devices` contains a driver for a different device. Currently supported devices are:

 - `eeprom/24LC16`: 1Kbi EEPROM memory.
 - `rtc/*`: Real Time Clocks
 - `temperature/*`: Temperature and humidity sensors
 - `LTR559ALS`: proximity and light sensor
 - `MCP4018`: DAC

# Porting

As mentioned, this library is hardware agnostic and requires an hardware abstraction layer in the form of the `i2c_transfer` callback to work.
In practice this means instantiating the `i2c_driver_t` structure.

```
const i2c_driver_t sht31_driver =
{ .device_address = 0x88, .i2c_transfer = esp_idf_i2c_port_transfer };
```

`device_address` and `i2c_transfer` are the only two mandatory fields. The device address is then passed to `i2c_transfer` invokations during
the driver's work.

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

## TODO

    - Add tests for al peripherals