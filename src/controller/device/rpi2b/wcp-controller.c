//
// Created by master on 5/7/16.
//
#include <wcp-controller.h>
#include <i2c-rpi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// make get_status and set_status thread-safe
extern void set_get_status_lock();
extern void set_get_status_unlock();

int fc_wcp_send_int8(unsigned long port, uint8_t data) {
    return rpi_i2c_write_byte(port, data);
}

int fc_wcp_recv_int8(unsigned long port, uint8_t *data) {
    return rpi_i2c_read_byte(port, data);
}

int fc_wcp_send_int32(unsigned long port, uint32_t data) {
    // uint8_t *send_data = (uint8_t*)&data;
    // int i;
    // for (i = 0; i < 4; ++i) {
    //     if (fc_wcp_send_int8(port, send_data[i]) != 0) {
    //         return -1;
    //     }
    // }
    // return 0;
    return rpi_i2c_write_block(port, 4, (uint8_t*)&data);
}

int fc_wcp_recv_int32(unsigned long port, uint32_t *data) {
    uint8_t *recv_data = (uint8_t*)data;
    int i;
    for (i = 0; i < 4; ++i) {
        if (fc_wcp_recv_int8(port, recv_data + i) != 0) {
            return -1;
        }
    }
    return 0;
}
int fc_wcp_send_block(unsigned long port, uint32_t *size, uint8_t *data) {
    // unsigned long i;
    // for (i = 0; i < *size; ++i) {
    //     if (fc_wcp_send_int8(port, data[i]) != 0) {
    //         *size = i;
    //         return -1;
    //     }
    // }
    // return 0;
    return rpi_i2c_write_block(port, *size, data);
}

int fc_wcp_read_block(unsigned long port, uint32_t *size, uint8_t *data) {
    unsigned long i;
    for (i = 0; i < *size; ++i) {
        if (fc_wcp_recv_int8(port, data + i) != 0) {
            *size = i;
            return -1;
        }
    }
    return 0;
}

int fc_wcp_trans_init(unsigned long port, FREECOPTER_WCP_TRANS_HEADER_T *header) {
    int res;
    res = fc_wcp_send_int8(port, header->command);
    if (res != 0) {
        // printf("fc_wcp_send_int8 failed\n");
        return res;
    }
    if (header->command == FREECOPTER_WCP_COMMAND_SET_STATUS) {
        res = fc_wcp_send_int32(port, header->data_length);
    } else if(header->command == FREECOPTER_WCP_COMMAND_GET_STATUS) {
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
    set_get_status_lock();

    res = fc_wcp_trans_init(port, &header);
    if (res != 0) {
        // printf("get status fc_wcp_trans_init failed\n");
        set_get_status_unlock();
        return res;
    } else {
        // printf("data length: %d\n", header.data_length);
    }
    buf = malloc(header.data_length);
    res = fc_wcp_read_block(port, &header.data_length, (uint8_t*)buf);
    if (res != 0) {
        // printf("fc_wcp_read_block failed\n");
        set_get_status_unlock();
        return res;
    }
    if (status == NULL) status = buf;
    else {
        truncated_channel_n = status->channel_n > buf->channel_n ? buf->channel_n : status->channel_n;
        status->channel_n = truncated_channel_n;
        memcpy(status->channel, buf->channel, truncated_channel_n * sizeof(FREECOPTER_WCP_CHANNEL_T));
        free(buf);
    }
    set_get_status_unlock();
    return res;
}
int fc_wcp_set_status(unsigned long port, FREECOPTER_WCP_STATUS_T *status) {
    int res;
    FREECOPTER_WCP_TRANS_HEADER_T header;

    header.command = FREECOPTER_WCP_COMMAND_SET_STATUS;
    header.data_length = sizeof(FREECOPTER_WCP_CHANNEL_T) * status->channel_n + 4;
    set_get_status_lock();

    res = fc_wcp_trans_init(port, &header);
    if (res != 0) {
        printf("set status fc_wcp_trans_init failed\n");
        set_get_status_unlock();
        return res;
    }
    printf("sending data...\n");
    res = fc_wcp_send_block(port, &header.data_length, (uint8_t*)status);
    if (res != 0) {
        printf("send block data failed. %d bytes sended\n", header.data_length);
    }
    set_get_status_unlock();
    return res;
}

void fc_wcp_wrapper_reset(unsigned long port) {
    // if the wrapper is in recv status, send byte request in wrong status will make it idle.
    fc_wcp_recv_int8(port, NULL);
    // if the wrapper is in send status, write byte will reset its status.
    // command_none indecate to idle status.
    fc_wcp_send_int8(port, FREECOPTER_WCP_COMMAND_NONE);
}
