#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <wcp-controller.h>
#include "i2c-rpi.h"
#include "utilities.h"


int main() {
	int fd;
    ConsoleCanvas cc;

	unsigned char buffer[60] = {0};
	int num;
    FREECOPTER_WCP_STATUS_T status;

    fd = rpi_i2c_open("/dev/i2c-1", 0x15);
    if (fd < 0) {
        printf("open i2c device failed\n");
        exit(1);
    }

	while(1) {
        if (fc_wcp_get_status(fd, &status) != 0) {
            printf("get status failed\n");
        } else {
            cc.refresh(dump_status(&status));
        }
        usleep(400000);
	}
	return 0;
}
