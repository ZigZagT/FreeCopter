#ifnedf FREECOPTER_I2C_RPI_H
#define FREECOPTER_I2C_RPI_H

#include <stdint.h>

int fc_rpi_i2c_open(char* path, uint8_t addr);
int fc_rpi_i2c_read_byte(int file, uint32_t *data);
int fc_rpi_i2c_write_byte(int file, uint32_t data);

#endif
