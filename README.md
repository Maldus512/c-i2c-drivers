# I2C Peripheral library

A collection of I2C drivers for various integrated circuits.
Immediately pluggable as ESP-IDF component.


## Zen

This is a collection of drivers for various I2C integrated circuits. It includes memories, clocks and sensors.
The code is hardware indipendent: all drivers rely on an interface-like struct (`i2c_driver_t`) with function pointers to interact with the underlying I2C network.

This means that to actually use this library a few implementations are required. One must instance a struct as such:

```c
i2c_driver_t driver = {
    .device_address = DEVICE_ADDRESS, // I2C device address
    .i2c_transfer = my_i2c_transfer_function, // a function that writes bytes to the I2C network and reads the required response.
    .ack_polling = my_ack_polling_function, // a function that performs ack polling. Useful for eeproms, not strictly required.
    .delay_ms = my_delay_function, // a function that suspends execution for the required number of milliseconds. Only required for some drivers.
    . arg = argument_pointer,   // a void pointer that is passed to i2c_transfer and ack_polling function. Serves as instantiation (e.g. when working with different I2C networks on the same device)
    }:
```

The `i2c_port` folder contains some hardware implementations.


## TODO

- Raise all code level up to standard (e.g. PIC ports)