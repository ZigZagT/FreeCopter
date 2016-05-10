#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wcp-wrapper.h>
#include "i2c_rw.h"

#define __length__      200
static uint8_t __send_pending[__length__];
static uint8_t __send_next[__length__];
static uint8_t __recv_pending[__length__];
static uint8_t __recv_fin[__length__];

void I2C_SEND_INIT(I2C_SEND_T* target, uint32_t buf_len) {
    memset(target, 0, sizeof(I2C_SEND_T));

    // target->pending_data = malloc(buf_len);
    // target->next_data = malloc(buf_len);

    // memset(target->pending_data, 0, buf_len);
    // memset(target->next_data, 0, buf_len);

    // target->pending_data_length = buf_len;
    // target->next_data_capcity = buf_len;

    target->pending_data = __send_pending;
    target->next_data = __send_next;
    memset(target->pending_data, 0, __length__);
    memset(target->next_data, 0, __length__);

    target->pending_data_capcity = __length__;
    target->next_data_capcity = __length__;
}
void I2C_RECV_INIT(I2C_RECV_T* target, uint32_t buf_len) {
    memset(target, 0, sizeof(I2C_RECV_T));

    // target->pending_data_buf = malloc(buf_len);
    // target->recv_data_buf = malloc(buf_len);

    // memset(target->pending_data_buf, 0, buf_len);
    // memset(target->recv_data_buf, 0, buf_len);

    // target->pending_data_length = buf_len;
    // target->recv_data_capcity = buf_len;

    target->pending_data_buf = __recv_pending;
    target->recv_data_buf = __recv_fin;
    memset(target->pending_data_buf, 0, __length__);
    memset(target->recv_data_buf, 0, __length__);
    target->pending_data_capcity = __length__;
    target->recv_data_capcity = __length__;
}

void I2C_SEND_RESET(I2C_SEND_T* target) {
    target->pending_data_index = 0;
}
void I2C_RECV_RESET(I2C_RECV_T* target) {
    target->pending_data_index = 0;
}

int I2C_Set_Send_Data(I2C_SEND_T* target, uint8_t* data, uint32_t* len) {
    // Mutex_Lock(&(target->next_data_mutex));
    if (Mutex_Try_Lock(&(target->next_data_mutex)) != 0) {
        *len = 0;
        return I2C_ERROR_LOCK_MUTEX_FAILED;
    }
    uint32_t truncted_len = *len > target->next_data_capcity ? target->next_data_capcity : *len;
    memcpy(target->next_data, data, truncted_len);
    target->next_data_length = truncted_len;
    *len = truncted_len;
    I2C_CLEAR_FLAG(target, I2C_FLAG_FLUSH);

    Mutex_Unlock(&(target->next_data_mutex));
    return 0;
}
int I2C_Get_Recv_Data(I2C_RECV_T* target, uint8_t* buf, uint32_t* len) {
    //Mutex_Lock(&(target->recv_data_mutex));
    if (Mutex_Try_Lock(&(target->recv_data_mutex)) != 0) {
        *len = 0;
        return I2C_ERROR_LOCK_MUTEX_FAILED;
    }
    uint32_t truncted_len = *len > target->recv_data_length ? target->recv_data_length : *len;
    memcpy(buf, target->recv_data_buf, truncted_len);
    *len = truncted_len;
    I2C_CLEAR_FLAG(target, I2C_FLAG_FLUSH);

    Mutex_Unlock(&(target->recv_data_mutex));
    return 0;
}
int I2C_Flush_Send_Data(I2C_SEND_T* target) {
    if (Mutex_Try_Lock(&(target->next_data_mutex)) != 0) {
        return I2C_ERROR_LOCK_MUTEX_FAILED;
    }
    if (I2C_TEST_FLAG(target, I2C_FLAG_FLUSH) && I2C_TEST_FLAG(target, I2C_FLAG_SEND_REPEAT)) {
        target->pending_data_index = 0;
        Mutex_Unlock(&(target->next_data_mutex));
        return 0;
    }
    if (I2C_TEST_FLAG(target, I2C_FLAG_FLUSH) && !I2C_TEST_FLAG(target, I2C_FLAG_FORCE_FLUSH)) {
        Mutex_Unlock(&(target->next_data_mutex));
        return I2C_ERROR_FLAG_FLUSH_IS_SET;
    }
    uint32_t truncted_len = target->pending_data_capcity > target->next_data_length ? target->next_data_length : target->pending_data_capcity;
    memcpy(target->pending_data, target->next_data, truncted_len);
    target->pending_data_length = truncted_len;
    target->pending_data_index = 0;
    I2C_SET_FLAG(target, I2C_FLAG_FLUSH);

    //printf("I2C_Flush_Send_Data: [%s]\n", target->pending_data);
    Mutex_Unlock(&(target->next_data_mutex));
    return 0;
}
int I2C_Flush_Recv_Data(I2C_RECV_T* target) {
    if (Mutex_Try_Lock(&(target->recv_data_mutex)) != 0) {
        //printf("I2C_Flush_Recv_Data failed: Mutex_Try_Lock failed\n");
        return I2C_ERROR_LOCK_MUTEX_FAILED;
    }
    if (I2C_TEST_FLAG(target, I2C_FLAG_FLUSH) && !I2C_TEST_FLAG(target, I2C_FLAG_FORCE_FLUSH)) {
        Mutex_Unlock(&(target->recv_data_mutex));
        //printf("I2C_Flush_Recv_Data failed: I2C_TEST_FLAG failed\n");
        return I2C_ERROR_FLAG_FLUSH_IS_SET;
    }
    //printf("I2C Flush Recv Data:\n\tlen:\t%d\n\tfirst:\t0x%X\n\n", target->pending_data_index, target->pending_data_buf[0]);
    uint32_t truncted_len = target->pending_data_index > target->recv_data_capcity ? target->recv_data_capcity : target->pending_data_index;
    memcpy(target->recv_data_buf, target->pending_data_buf, truncted_len);
    target->recv_data_length = truncted_len;
    target->pending_data_index = 0;

    //printf("Flush: old flag: 0x%X\n", target->flag);
    I2C_SET_FLAG(target, I2C_FLAG_FLUSH);
    //printf("Flush: new flag: 0x%X\n", target->flag);

    Mutex_Unlock(&(target->recv_data_mutex));
    return 0;
}
int I2C_Send_Byte(I2C_SEND_T* target, I2C_T* port, uint8_t* data) {
    uint8_t pdata;
    int temp;

    if (target->pending_data_index >= target->pending_data_length) {
        temp = I2C_Flush_Send_Data(target);
        if (I2C_TEST_FLAG(target, I2C_FLAG_SEND_REPEAT) && temp == I2C_ERROR_FLAG_FLUSH_IS_SET) {
            target->pending_data_index = 0;
        } else {
            return temp;
        }
        if (target->pending_data_length == 0) {
            return I2C_ERROR_DATA_BLOCK_EMPTY;
        }
    }
    pdata = target->pending_data[target->pending_data_index];
    I2C_SET_DATA(port, pdata);
    ++(target->pending_data_index);
    //I2C_SET_CONTROL_REG(port, I2C_I2CON_SI_AA);
    if (data != NULL) {
        *data = pdata;
    }

    if (target->pending_data_index >= target->pending_data_length) {
        return I2C_NOTICE_LAST_DATA_SENT;
    }
    return 0;
}
int I2C_Recv_Byte(I2C_RECV_T* target, I2C_T* port, uint8_t* data) {
    uint8_t pdata;
    int temp;
    pdata = I2C_GET_DATA(port);
    if (data != NULL) {
        *data = pdata;
    }
    if(target->pending_data_index >= target->pending_data_capcity) {
        temp = I2C_Flush_Recv_Data(target);
        if ( temp != 0) {
            return temp;
        }
    }
    target->pending_data_buf[target->pending_data_index] = pdata;
    ++(target->pending_data_index);
    //I2C_SET_CONTROL_REG(port, I2C_I2CON_SI_AA);
    return 0;
}
