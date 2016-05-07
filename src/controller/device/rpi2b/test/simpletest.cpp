#include <wcp-controller.h>
#include <iostream>
#include <stdio.h>

#include "i2c-rpi.h"

void dump_status(FREECOPTER_WCP_STATUS_T* status) {

}

int main(int argc, char** argv) {
    int fd = rpi_i2c_open("/dev/i2c-1", 0x15);
    FREECOPTER_WCP_STATUS_T * status;
    fc_wcp_get_status(fd, status);
    return 0;
}
