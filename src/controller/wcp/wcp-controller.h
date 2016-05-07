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
