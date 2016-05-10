#include <wcp-controller.h>
#include "i2c-rpi.h"
#include "utilities.h"

#include <ZTunnel.h>

#include <sstream>
#include <iomanip>
#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <curses.h>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::setw;
using std::runtime_error;
using std::left;
using std::right;

string Additional_Msg;
size_t dump_height = 7;
ConsoleCanvas cc;
int fd;
FREECOPTER_WCP_STATUS_T *status;

void Drawer() {
    FREECOPTER_WCP_STATUS_T status;
    ::status = &status;

    fd = rpi_i2c_open("/dev/i2c-1", 0x15);
    if (fd < 0) {
        printf("open i2c device failed\n");
        exit(1);
    }

	while(1) {
        if (fc_wcp_get_status(fd, &status) != 0) {
            printf("get status failed\n");
        } else {
            cc.refresh(dump_status(&status) + Additional_Msg);
        }
        //usleep(400000);
        sleep(1);
	}
}

void Listener() {
    std::ostringstream ss;
    size_t height = cc.get_screen_size().height_char - dump_height;
    for (int i = 0; i < height - 2; ++i) {
        ss << endl;
    }
    string prefix_msg = ss.str();
    ss << "press A/D key to control throttle(channel 3)\n";
    Additional_Msg = ss.str();

    while(true) {
        ss = std::ostringstream();
        FREECOPTER_WCP_STATUS_T status_b = *status;

        switch (getchar()) {
            case 'a':
            case 'A':
            status_b.channel[3].value -= 100;
            status_b.channel[3].signal_source = FREECOPTER_WCP_CHANNEL_SIGSOURCE_CONTROLLED;
            ss << "A received\n";
            break;
            case 'd':
            case 'D':
            status_b.channel[3].value += 100;
            status_b.channel[3].signal_source = FREECOPTER_WCP_CHANNEL_SIGSOURCE_CONTROLLED;
            ss << "D received\n";
            break;
        }
        if (fc_wcp_set_status(fd,&status_b) != 0) {
            ss << "set status failed\n";
        } else {
            ss << dump_status(&status_b);
        }
        // if (getch() == '\033') { // if the first value is esc
        //     ss << "ANSI escape codes: " << getch();
        //     switch(getch()) { // the real value
        //     case 'A':
        //         // code for arrow up
        //         break;
        //     case 'B':
        //         // code for arrow down
        //         break;
        //     case 'C':
        //         // code for arrow right
        //
        //     case 'D':
        //         // code for arrow left
        //
        //     default:
        //         break;
        //     }
        // }
        Additional_Msg = ss.str();
    }
}

int main(int argc, char** argv) {
    ZTunnel::ZThread t;
    t.run(ZTunnel::ZTask(Drawer));

    sleep(2);

    Listener();
    t.join();

    return 0;
}
