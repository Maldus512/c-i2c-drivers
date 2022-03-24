#ifndef I2C_COMMON_H_INCLUDED
#define I2C_COMMON_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define I2C_READ_ADDR(x)  (x | 0x01)
#define I2C_WRITE_ADDR(x) (x & 0xFE)

#define BCD2BIN(x) ((((x) >> 4) & 0xF) * 10 + ((x)&0xF))
#define BIN2BCD(x) ((((x) / 10) << 4) | ((x) % 10))


typedef struct {
    // 8-bit Address of the device. The least significant bit should be zero, and it is used to distinguish
    // between read and write operations
    uint8_t device_address;

    /* Callback for hardware i2c transfer operation
     *  Sends an array of bytes to the specified device and then reads back another array.
     *
     *  devaddr: Device address
     *  writebuf: buffer of bytes to be written to the device. It can be NULL (in which case no data should be written)
     *  writelen: number of bytes to be written
     *  readbuf: buffer to be filled with bytes read from the device. It can be NULL (in which case no data should be
     * read) readlen: number of bytes to be read
     *  arg: extra user argument
     *
     *  return: 0 if everything went well, something else otherwise (the error is propagated)
     */
    int (*i2c_transfer)(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf, size_t readlen,
                        void *arg);

    /* Callback for an address polling operation
     * Some devices (mainly I2C eeproms) support address polling: when queried for a write operation they
     * respond with a NACK bit to signal they are not ready yet (i.e. still writing from a previous command).
     * When they are ready, they respond with ACK again.
     * This is a peculiar transaction that is not allowed on every system. It is trivial to implement when bit-banging
     * but dependent on the hardware when using a module.
     * It can easily be replaced by a suitably long time delay.
     *
     *  devaddr: Device address
     *  arg: extra user argument
     */
    void (*ack_polling)(uint8_t devaddr, void *arg);

    /**
     * @brief Delay function. Useful for synchronous operations on some devices
     *
     */
    void (*delay_ms)(unsigned long ms);

    /**
     * @brief Extra user argument
     *
     */
    void *arg;
} i2c_driver_t;


typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t wday;
    uint8_t month;
    uint8_t year;
} rtc_time_t;


/* Write a specific register
 * Many I2C devices work with internal addressable registers. This functions simply executes a transaction containing
 * the register address as the first written byte.
 *
 * driver: the i2c driver structure
 * reg: register address
 * data: bytes to write at the register address
 * len: number of bytes to write
 *
 * return: 0 if everything went well, something else otherwise (propagated from the callbacks)
 */
int i2c_write_register(i2c_driver_t driver, uint8_t reg, uint8_t *data, size_t len);


/* Read a specific register
 * Many I2C devices work with internal addressable registers. This functions simply executes a transaction containing
 * the register address as the first written byte, and then reading its contents.
 *
 * driver: the i2c driver structure
 * reg: register address
 * data: bytes to read at the register address
 * len: number of bytes to read
 *
 * return: 0 if everything went well, something else otherwise (propagated from the callbacks)
 */
int i2c_read_register(i2c_driver_t driver, uint8_t reg, uint8_t *data, size_t len);

#endif
