#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "posix_eeprom_file_i2c_port.h"


static int get_file_size(char *filename);
static int pad_file(char *filename, size_t padding);
static int overwrite_file(char *filename, size_t start, uint8_t *buffer, size_t len);
static int read_file(char *filename, size_t start, uint8_t *buffer, size_t len);


// Current eeprom address
static size_t current_address = 0;


int posix_eeprom_file_i2c_u8addr_port_transfer(uint8_t devaddr, uint8_t *writebuf, size_t writelen, uint8_t *readbuf,
                                               size_t readlen, void *arg) {
    char *filename = arg;

    // Take 3 bits from the device address + the first write buffer byte
    if (writelen > 0 && writebuf != NULL) {
        size_t start = 0;

        if (devaddr == I2C_WRITE_ADDR(devaddr)) {
            // Random read, update address
            current_address = ((devaddr & 0xE) << 8) | writebuf[start++];
        }

        if (writelen > start) {
            int res = get_file_size(filename);
            if (res < 0) {
                return -1;
            }
            size_t size = (size_t)res;

            if (size < current_address) {
                if (pad_file(filename, current_address - size)) {
                    return -1;
                }
            }

            res = overwrite_file(filename, current_address, &writebuf[start], writelen - start);
            if (res < 0) {
                return -1;
            } else {
                // Update the current address
                current_address += (size_t)res;
            }
        } else {
            // Otherwise just update the address
        }
    }

    if (readlen > 0 && readbuf != NULL) {
        int res = get_file_size(filename);
        if (res < 0) {
            return -1;
        }
        size_t size = (size_t)res;

        if (size < current_address + readlen) {
            if (pad_file(filename, current_address + readlen - size)) {
                return -1;
            }
        }

        return (read_file(filename, current_address, readbuf, readlen) < 0);
    }

    return 0;
}


static int get_file_size(char *filename) {
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Could not open file %s: %s\n", filename, strerror(errno));
        return 0;
    }

    if (fseek(fp, 0L, SEEK_END)) {
        printf("fseek error: %s\n", strerror(errno));
        fclose(fp);
        return -1;
    }

    int res = ftell(fp);
    if (res < 0) {
        printf("ftell error: %s\n", strerror(errno));
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return res;
}


static int pad_file(char *filename, size_t padding) {
    FILE *fp = fopen(filename, "a");

    if (fp == NULL) {
        printf("Could not open file %s: %s\n", filename, strerror(errno));
        return -1;
    }

    uint8_t *buffer = malloc(padding);
    memset(buffer, 0xFF, padding);

    size_t written = 0;
    while (written < padding) {
        int res = fwrite(&buffer[written], 1, padding - written, fp);
        if (res < 0) {
            printf("Write error: %s\n", strerror(errno));
            fclose(fp);
            free(buffer);
            return -1;
        } else {
            written += (size_t)res;
        }
    }

    fclose(fp);
    free(buffer);
    return 0;
}


static int overwrite_file(char *filename, size_t start, uint8_t *buffer, size_t len) {
    FILE *fp = fopen(filename, "r+");

    if (access(filename, F_OK) == 0) {
        fp = fopen(filename, "r+");
    } else {
        fp = fopen(filename, "w");
    }

    if (fp == NULL) {
        printf("Could not open file %s: %s\n", filename, strerror(errno));
        return -1;
    }

    if (fseek(fp, start, SEEK_SET)) {
        printf("fseek error: %s\n", strerror(errno));
        fclose(fp);
        return -1;
    }

    size_t written = 0;
    while (written < len) {
        int res = fwrite(&buffer[written], 1, len - written, fp);
        if (res < 0) {
            printf("Write error: %s\n", strerror(errno));
            fclose(fp);
            return -1;
        } else {
            written += (size_t)res;
        }
    }

    fclose(fp);
    return (int)len;
}


static int read_file(char *filename, size_t start, uint8_t *buffer, size_t len) {
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Could not open file %s: %s\n", filename, strerror(errno));
        return -1;
    }

    if (fseek(fp, start, SEEK_SET)) {
        printf("fseek error: %s\n", strerror(errno));
        fclose(fp);
        return -1;
    }

    size_t read = 0;
    while (read < len) {
        int res = fread(&buffer[read], 1, len - read, fp);
        if (res < 0) {
            printf("Read error: %s\n", strerror(errno));
            fclose(fp);
            return -1;
        } else if (res == 0) {
            printf("File was not long enough");
            fclose(fp);
            return len;
        } else {
            read += (size_t)res;
        }
    }

    fclose(fp);
    return (int)len;
}