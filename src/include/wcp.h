//
// Created by master on 5/6/16.
//

#include <stdint.h>

#ifndef FREECOPTER_WCP_H
#define FREECOPTER_WCP_H


/*
 * translation layer goes here
 */

/*
 * Basic translate functions used to implement translation function.
 */
int fc_send_int8(unsigned long port, uint8_t data);
int fc_recv_int8(unsigned long port, uint8_t *data);
int fc_send_int32(unsigned long port, uint32_t data);
int fc_recv_int32(unsigned long port, uint32_t *data);

/*
 * WCP command definitions
 */
#define FREECOPTER_WCP_COMMAND_SEND_DATA                            1
#define FREECOPTER_WCP_COMMAND_RECV_DATA                            2

/*
 * WCP translation data structure definitions
 */
typedef struct {
    uint8_t read_write;
    uint32_t data_length;

} FREECOPTER_WCP_TRANS_HEADER_T;

/*
 * WCP translate calls
 */
int fc_wcp_trans_init(unsigned long port, FREECOPTER_WCP_TRANS_HEADER_T *header);











/*
 * protocol layer goes here
 */

/*
 * WCP command definitions
 */
#define FREECOPTER_WCP_COMMAND_GET_CHANNELS                         3
#define FREECOPTER_WCP_COMMAND_SET_CHANNELS                         4

/*
 * WCP channel definitions
 */
#define FREECOPTER_WCP_CHANNEL_AILERON                              1
#define FREECOPTER_WCP_CHANNEL_ELEVATOR                             2
#define FREECOPTER_WCP_CHANNEL_THROTTLE                             3
#define FREECOPTER_WCP_CHANNEL_RUDDER                               4
#define FREECOPTER_WCP_CHANNEL_USER1                                5
#define FREECOPTER_WCP_CHANNEL_USER2                                6
#define FREECOPTER_WCP_CHANNEL_USER3                                7
#define FREECOPTER_WCP_CHANNEL_USER4                                8
#define FREECOPTER_WCP_CHANNEL_USER5                                9

/*
 * WCP channel flag definitions
 */
#define FREECOPTER_WCP_FLAG_CHANNEL_FORWARD                         (1 << 0)
#define FREECOPTER_WCP_FLAG_CHANNEL_CONTROLLED                      (1 << 1)

/*
 * WCP data structure definitions
 */
typedef struct {
    uint32_t name;
    uint32_t flag;
} FREECOPTER_WCP_CHANNEL_T;

typedef struct {
    uint32_t channel_n;
    FREECOPTER_WCP_CHANNEL_T* channel;
} FREECOPTER_WCP_STATUS_T;


/*
 * WCP calls
 */
//int fc_wcp_call(unsigned long port, uint32_t command, unsigned long data);
int fc_wcp_get_status(unsigned long port, FREECOPTER_WCP_STATUS_T *status);
int fc_wcp_set_status(unsigned long port, FREECOPTER_WCP_STATUS_T *status);

#endif //FREECOPTER_WCP_H
