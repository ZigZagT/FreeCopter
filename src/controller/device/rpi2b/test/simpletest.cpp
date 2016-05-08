#include <wcp-controller.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>

#include "i2c-rpi.h"

void dump_status(FREECOPTER_WCP_STATUS_T* status) {

}

int main(int argc, char** argv) {
    int fd = rpi_i2c_open("/dev/i2c-1", 0x15);
    int res;
    if (fd < 0) {
        printf("open i2c device failed\n");
        exit(1);
    }
    FREECOPTER_WCP_STATUS_T * status;
    while(true) {
        sleep(100);
        res = fc_wcp_get_status(fd, status);
        if (res != 0) {
            printf("get status filed %d, errno %d, %s\n", res, errno, strerror(errno));
        }
    }
    return 0;
}
