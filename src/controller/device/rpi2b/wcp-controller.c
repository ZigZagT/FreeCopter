//
// Created by master on 5/7/16.
//
#include <wcp-controller.h>
#include <i2c-rpi.h>
#include <string.h>
#include <stdlib.h>

int fc_wcp_send_int8(unsigned long port, uint8_t data) {
    return fc_rpi_i2c_write_byte(port, data);
}

int fc_wcp_recv_int8(unsigned long port, uint8_t *data) {
    return fc_rpi_i2c_read_byte(port, data);
}

int fc_wcp_send_int32(unsigned long port, uint32_t data) {
    uint8_t *send_data = (uint8_t*)&data;
    int i;
    for (i = 0; i < 4; ++i) {
        if (fc_wcp_send_int8(port, send_data[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

int fc_wcp_recv_int32(unsigned long port, uint32_t *data) {
    uint8_t *recv_data = (uint8_t*)data;
    int i;
    for (i = 0; i < 4; ++i) {
        if (fc_wcp_send_int8(port, recv_data + i) != 0) {
            return -1;
        }
    }
    return 0;
}
int fc_wcp_send_block(unsigned long port, unsigned long size, uint8_t *data) {
    unsigned long i;
    for (i = 0; i < size; ++i) {
        if (fc_wcp_send_int8(port, data[i]) != 0) {
            return -1;
        }
    }
    return 0;
}
int fc_wcp_read_block(unsigned long port, unsigned long size, uint8_t *data) {
    unsigned long i;
    for (i = 0; i < size; ++i) {
        if (fc_wcp_send_int8(port, data + i) != 0) {
            return -1;
        }
    }
    return 0;
}

int fc_wcp_trans_init(unsigned long port, FREECOPTER_WCP_TRANS_HEADER_T *header) {
    int res;
    res = fc_wcp_send_int8(port, header->command);
    if (res != 0) return res;
    if (header->command == FREECOPTER_WCP_COMMAND_SEND_DATA) {
        res = fc_wcp_send_int32(port, header->data_length);
    } else if(header->read_write_command == FREECOPTER_WCP_COMMAND_RECV_DATA) {
        res = fc_wcp_recv_int32(port, &(header->data_length));
    } else {
        return -1;
    }
    return res;
}

int fc_wcp_get_status(unsigned long port, FREECOPTER_WCP_STATUS_T *status) {
    int res;
    uint32_t truncated_channel_n;
    FREECOPTER_WCP_STATUS_T *buf;
    FREECOPTER_WCP_TRANS_HEADER_T header;

    header.command = FREECOPTER_WCP_COMMAND_GET_STATUS;
    res = fc_wcp_trans_init(port, &header);
    if (res != 0) return res;
    buf = malloc(header.data_length);
    res = fc_wcp_read_block(port, header.data_length, buf);
    if (res != 0) return res;
    if (status == NULL) status = buf;
    else {
        truncated_channel_n = status->channel_n > buf->channel_n ? buf->channel_n : status->channel_n;
        status->channel_n = truncated_channel_n;
        memcpy(status->channel, buf->channel, truncated_channel_n * sizeof(FREECOPTER_WCP_CHANNEL_T));
        free(buf);
    }
    return res;
}
int fc_wcp_set_status(unsigned long port, FREECOPTER_WCP_STATUS_T *status) {
    int res;
    FREECOPTER_WCP_TRANS_HEADER_T header;

    header.command = FREECOPTER_WCP_COMMAND_SET_STATUS;
    header.data_length = sizeof(FREECOPTER_WCP_CHANNEL_T) * status->channel_n + 4;
    res = fc_wcp_trans_init(port, &header);
    if (res != 0) return res;
    res = fc_wcp_write_block(port, header.data_length, status);
    return res;
}
