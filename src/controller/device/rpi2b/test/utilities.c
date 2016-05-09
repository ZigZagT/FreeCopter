#include <stdio.h>
#include "utilities.h"

void dump_status(FREECOPTER_WCP_STATUS_T* status) {
    printf("dump status %d\n", status->channel_n);
    for (int i = 0; i < status->channel_n; ++i) {
        printf("\tchannel %d\tvalue %d\t\tsig %d\n", status->channel[i].name, status->channel[i].value, status->channel[i].signal_source);
    }
    printf("dump completed.\n");
}
