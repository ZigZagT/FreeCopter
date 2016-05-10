//
// Created by master on 5/6/16.
//

#ifndef FREECOPTER_WCP_CONTROLLER_H
#define FREECOPTER_WCP_CONTROLLER_H

#include <stdint.h>
#include <wcp.h>

/*
 * translation layer goes here
 */
 int fc_wcp_send_int8(unsigned long port, uint8_t data);
 int fc_wcp_recv_int8(unsigned long port, uint8_t *data);
 int fc_wcp_send_int32(unsigned long port, uint32_t data);
 int fc_wcp_recv_int32(unsigned long port, uint32_t *data);
 int fc_wcp_send_block(unsigned long port, unsigned long *size, uint8_t *data);
 int fc_wcp_read_block(unsigned long port, unsigned long *size, uint8_t *data);
/*
 * WCP translate calls
 */
int fc_wcp_trans_init(unsigned long port, FREECOPTER_WCP_TRANS_HEADER_T *header);

/*
 * protocol layer goes here
 */
/*
 * WCP calls
 */
//int fc_wcp_call(unsigned long port, uint32_t command, unsigned long data);
int fc_wcp_get_status(unsigned long port, FREECOPTER_WCP_STATUS_T *status);
int fc_wcp_set_status(unsigned long port, FREECOPTER_WCP_STATUS_T *status);


#endif //FREECOPTER_WCP_CONTROLLER_H
