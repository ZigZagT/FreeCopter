#include <wcp-controller.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "i2c-rpi.h"
#include "utilities.h"

int main() {
	int i;
	int fd;
	unsigned char buffer[60] = {0};
	int num, val;
    FREECOPTER_WCP_STATUS_T status;

    fd = rpi_i2c_open("/dev/i2c-1", 0x15);
    if (fd < 0) {
        printf("open i2c device failed\n");
        exit(1);
    } else {
        printf("i2c device opened %d\n", fd);
    }

	while(1) {
		printf("\n> ");
		scanf("%59s", buffer);
		if (buffer[0] == 'r') {
            scanf("%d", &num);
            int temp = 0;
            for (int i = 0; i < num; ++i) {
                if(rpi_i2c_read_byte(fd, buffer + i) != 0) {
                    printf("\nread failed\n");
                    temp = 1;
                    break;
                } else {
                    printf("%d ", buffer[i]);
                }
                if (i % 24 == 0) {
                    usleep(100);
                }
            }
            if (temp) {
                printf("\nread finished.\n");
            }

		} else if (buffer[0] == 'w') {
            scanf("%d", &num);
			printf("preparing to send %d\n", num);
			//length = strlen(buffer) + 1;
			if (rpi_i2c_write_byte(fd, num) != 0) {
				printf("write failed with %d-%s\n", errno, strerror(errno));
			} else {
				printf("send data %d\n", num);
			}
		} else if (buffer[0] == 'd') {
            if (fc_wcp_get_status(fd, &status) != 0) {
                printf("get status failed\n");
            } else {
                printf(dump_status(&status).c_str());
            }
        } else if (buffer[0] == 'o') {
            close(fd);
            fd = rpi_i2c_open("/dev/i2c-1", 0x15);
        } else if (buffer[0] == 's') {
            scanf("%d %d", &num, &val);
            if (fc_wcp_get_status(fd, &status) != 0) {
                printf("get status failed\n");
            } else {
                status.channel[num].value = val;
                status.channel[num].signal_source = FREECOPTER_WCP_CHANNEL_SIGSOURCE_CONTROLLED;
                if (fc_wcp_set_status(fd, &status) != 0) {
                    printf("set status failed\n");
                } else {
                    printf("send status %s", dump_status(&status).c_str());
                }
            }

        } else if (buffer[0] == 'e') {
            printf("wrapper status reset...\n");
            fc_wcp_wrapper_reset(fd);
        }
        else {
			printf(  "command:\n"
                    "\th                       show this help message\n"
                    "\tr <length>              read bytes\n"
                    "\tw <data>                write single byte\n"
                    "\td                       dump status\n"
                    "\to                       reopen interface\n"
                    "\ts <channel> <value>     set channel value\n"
                    "\te                       reset wrapper transfer status\n");
		}
	}
	return 0;
}
