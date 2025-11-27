#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "../../i2c_common/i2c_common.h"


int linux_i2c_port_transfer(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf, size_t readlen,
                            void *arg) {
    int file_i2c;

    //----- OPEN THE I2C BUS -----
    char *filename = (char *)arg;
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        printf("Failed to open the i2c bus: %s\n", strerror(errno));
        close(file_i2c);
        return -1;
    }

    if (ioctl(file_i2c, I2C_SLAVE, devaddr >> 1) < 0) {
        printf("Failed to acquire bus access and/or talk to slave: %s\n", strerror(errno));
        close(file_i2c);
        return -1;
    }

    if (writebuf && writelen > 0) {
        if (write(file_i2c, writebuf, writelen) !=
            (ssize_t)writelen)     // write() returns the number of bytes actually written, if it doesn't match then an
                                   // error occurred (e.g. no response from the device)
        {
            printf("Failed to write to the i2c bus: %s\n", strerror(errno));
            close(file_i2c);
            return -1;
        }
    }

    if (readbuf && readlen > 0) {
        //----- READ BYTES -----
        if (read(file_i2c, readbuf, readlen) !=
            (ssize_t)readlen)     // read() returns the number of bytes actually read, if it doesn't
                                  // match then an error occurred (e.g. no response from the device)
        {
            // ERROR HANDLING: i2c transaction failed
            printf("Failed to read from the i2c bus: %s\n", strerror(errno));
            close(file_i2c);
            return -1;
        }
    }

    close(file_i2c);
    return 0;
}
