//
// Created by master on 5/7/16.
//
#include <string.h>
#include <wcp-wrapper.h>
#include <stdio.h>
#include "define.h"
#include "i2c_rw.h"
#include "wcp-helper.h"

uint8_t fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_IDLE;
FREECOPTER_WCP_TRANS_HEADER_T fc_wcp_status_transfer_header;
uint32_t fc_wcp_status_transfer_progress = 0;

uint32_t fc_wcp_status_timeout_count;
uint32_t fc_wcp_status_timeout_reload;

FREECOPTER_WCP_STATUS_T fc_wcp_status_channels;
uint32_t fc_wcp_status_channels_control_expires;
uint32_t fc_wcp_status_channels_control_expires_reload;

int fc_wcp_reset_transfer_status() {
    fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_IDLE;
    memset(&fc_wcp_status_transfer_header, 0, sizeof(fc_wcp_status_transfer_header));
    fc_wcp_status_transfer_progress = 0;

    fc_wcp_status_timeout_count = fc_wcp_status_timeout_reload;
    return 0;
}

int fc_wcp_reset_channel_status() {
    uint32_t i;
    for (i = 0; i < fc_wcp_status_channels.channel_n; ++i) {
        fc_wcp_status_channels.channel[i].signal_source = FREECOPTER_WCP_CHANNEL_SIGSOURCE_FORWARD;
    }
    return 0;
}
int fc_wcp_update_channel_status(unsigned long recv_control) {
    uint32_t size = sizeof(fc_wcp_status_channels);
    fc_wcp_status_channels_control_expires = fc_wcp_status_channels_control_expires_reload;
    return I2C_Get_Recv_Data((I2C_RECV_T*)recv_control, (uint8_t*)&fc_wcp_status_channels, &size);
}

void inline fc_wcp_timeout_reload() {
    fc_wcp_status_timeout_count = fc_wcp_status_timeout_reload;
}

int fc_wcp_loop() {
    --fc_wcp_status_timeout_count;
    --fc_wcp_status_channels_control_expires;

    if(fc_wcp_status_timeout_count == 0) {
        Disable_I2C0_INT();
        fc_wcp_reset_transfer_status();
        Enable_I2C0_INT();
    }
    if(fc_wcp_status_channels_control_expires == 0) {
        Disable_I2C0_INT();
        fc_wcp_reset_channel_status();
        fc_wcp_reset_transfer_status();
        Enable_I2C0_INT();
    }
    return 0;
}

int fc_wcp_send_data(unsigned long fc_args) {
    fc_wcp_pre_send_data_args_t *args = (fc_wcp_pre_send_data_args_t*)fc_args;
    int res;
    uint32_t temp;

    // check current transfer status
    switch (fc_wcp_status_transfer) {
        case FREECOPTER_WCP_STATUS_TRANSFER_SEND_HEADER:
        case FREECOPTER_WCP_STATUS_TRANSFER_SEND:
        // status ok
        break;
        default:
        // status ERROR
        printf("send in invalid status\n");
        return -1;

    }

    // update send progress
    switch (args->status) {
        case I2C_STATUS_SLAVE_SEND_DATA_NACK:   // previous sent data received by master, and won't send new data
        case I2C_STATUS_SLAVE_SEND_LAST_DATA_ACK:   // previous sent data received by master, and won't send new data
        case I2C_STATUS_SLAVE_SEND_DATA_ACK:    // previous sent data received by master, and new data will be sent.
        ++fc_wcp_status_transfer_progress;
        break;
        default:
        break;
    }

    // check if the send progress is completed..
    switch (fc_wcp_status_transfer) {
        case FREECOPTER_WCP_STATUS_TRANSFER_SEND_HEADER:
        if(fc_wcp_status_transfer_progress == 4) {
            //printf("send header finished.\n");
            fc_wcp_status_transfer_progress = 0;
            fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_SEND;

            temp = sizeof(fc_wcp_status_channels);
            I2C_Set_Send_Data(args->target, (uint8_t*)&fc_wcp_status_channels, &temp);
            I2C_Flush_Send_Data(args->target);
        }
        break;
        case FREECOPTER_WCP_STATUS_TRANSFER_SEND:
        if(fc_wcp_status_transfer_progress == fc_wcp_status_transfer_header.data_length) {
            printf("send data finished.\n");
            fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_IDLE;
        }
        break;
        default:
        break;
    }

    // send new data if required.
    switch (args->status) {
        case I2C_STATUS_SLAVE_SEND_DATA_ACK:
        case I2C_STATUS_SLAVE_SEND_ADDR_ACK:
        res = I2C_Send_Byte(args->target, args->port, args->data);
        fc_wcp_timeout_reload();
        break;
        default:
        break;
    }

    return res;
}

int fc_wcp_post_send_data(unsigned long args) {
    ++fc_wcp_status_transfer_progress;
    return 0;
}
