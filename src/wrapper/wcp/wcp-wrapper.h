//
// Created by master on 5/6/16.
//

#ifndef FREECOPTER_WCP_WRAPPER_H
#define FREECOPTER_WCP_WRAPPER_H

#include <stdint.h>
#include <wcp.h>

/*
 * The WCP for wrapper is STATU MACHINE
 */

#define FREECOPTER_WCP_STATUS_TRANSFER_IDLE                             1
#define FREECOPTER_WCP_STATUS_TRANSFER_RECV_HEADER                      2
#define FREECOPTER_WCP_STATUS_TRANSFER_SEND_HEADER                      3
#define FREECOPTER_WCP_STATUS_TRANSFER_RECV                             4
#define FREECOPTER_WCP_STATUS_TRANSFER_SEND                             5

extern uint8_t fc_wcp_status_transfer;
extern FREECOPTER_WCP_TRANS_HEADER_T fc_wcp_status_transfer_header;
extern uint32_t fc_wcp_status_transfer_progress;
extern uint32_t fc_wcp_status_timeout_count;
extern uint32_t fc_wcp_status_timeout_reload;

extern FREECOPTER_WCP_STATUS_T fc_wcp_status_channels;
extern uint32_t fc_wcp_status_channels_control_expires;
extern uint32_t fc_wcp_status_channels_control_expires_reload;

int fc_wcp_reset_transfer_status();
int fc_wcp_reset_channel_status();
int fc_wcp_update_channel_status(unsigned long);
int fc_wcp_loop();

void inline fc_wcp_timeout_reload();
void inline fc_wcp_control_expires_reload();
int fc_wcp_send_data(unsigned long args);
int fc_wcp_recv_data(unsigned long args);

#endif //FREECOPTER_WCP_WRAPPER_H
