#ifndef FREECOPTER_I2C_RPI_H
#define FREECOPTER_I2C_RPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int rpi_i2c_open(const char* path, uint8_t addr);
int rpi_i2c_read_byte(int file, uint8_t *data);
int rpi_i2c_write_byte(int file, uint8_t data);

#ifdef __cplusplus
};
#endif

#endif
