//
// Created by master on 5/7/16.
//
#include <wcp-wrapper.h>
#include "i2c_rw.h"

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
}

int fc_wcp_reset_channel_status() {
    uint32_t i;
    for (i = 0; i < fc_wcp_status_channels.channel_n) {
        fc_wcp_status_channels.channel[i].signal_source = FREECOPTER_WCP_CHANNEL_SIGSOURCE_FORWARD;
    }
}
int fc_wcp_update_channel_status(unsigned long recv_control) {
    size_t size = sizeof(fc_wcp_status_channels);
    return I2C_Get_Recv_Data(recv_control, &fc_wcp_status_channels, &size);
    fc_wcp_status_channels_control_expires = fc_wcp_status_channels_control_expires_reload;
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
}
