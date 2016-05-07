//
// Created by master on 5/6/16.
//
#ifndef FREECOPTER_WCP_H
#define FREECOPTER_WCP_H

/*
 * translation layer goes here
 */
/*
 * WCP command definitions
 */
//#define FREECOPTER_WCP_COMMAND_SEND_DATA                            1
//#define FREECOPTER_WCP_COMMAND_RECV_DATA                            2
#define FREECOPTER_WCP_COMMAND_GET_STATUS                           3
#define FREECOPTER_WCP_COMMAND_SET_STATUS                           4
/*
 * WCP translation data structure definitions
 */
typedef struct {
    uint8_t command;
    uint32_t data_length;

} FREECOPTER_WCP_TRANS_HEADER_T;






/*
 * protocol layer goes here
 */

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
#define FREECOPTER_WCP_CHANNEL_SIGSOURCE_FORWARD                    0
#define FREECOPTER_WCP_CHANNEL_SIGSOURCE_CONTROLLED                 1

/*
 * WCP data structure definitions
 */
typedef struct {
    uint32_t name;
    uint32_t signal_source;
} FREECOPTER_WCP_CHANNEL_T;

typedef struct {
    uint32_t channel_n;
    FREECOPTER_WCP_CHANNEL_T channel[5];
} FREECOPTER_WCP_STATUS_T;


/*
 * Basic translate functions used to implement translation function.
 */
int fc_wcp_send_int8(unsigned long port, uint8_t data);
int fc_wcp_recv_int8(unsigned long port, uint8_t *data);
int fc_wcp_send_int32(unsigned long port, uint32_t data);
int fc_wcp_recv_int32(unsigned long port, uint32_t *data);
int fc_wcp_send_block(unsigned long port, unsigned long *size, uint8_t *data);
int fc_wcp_read_block(unsigned long port, unsigned long *size, uint8_t *data);
#endif //FREECOPTER_WCP_H
