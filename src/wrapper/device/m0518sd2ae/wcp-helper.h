#ifndef WCP_HELPER_H
#define WCP_HELPER_H

#include <stdint.h>
#include "i2c_rw.h"

typedef struct {
    uint32_t status;
    I2C_SEND_T* target;
    I2C_T* port;
    uint8_t *data;
} fc_wcp_pre_send_data_args_t;

typedef fc_wcp_pre_send_data_args_t fc_wcp_pre_recv_data_args_t;

#endif
