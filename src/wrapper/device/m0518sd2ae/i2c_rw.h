#ifndef __I2C_RW_H__
#define __I2C_RW_H__ 1

#include <stdint.h>
#include "M0518.h"
#include "define.h"
#include "mutex.h"

//  Indecate that data has just been flushed.
#define I2C_FLAG_FLUSH              (1 << 0)
//  Waiting while I2C_FLAG_FLUSH is not set.
// #define I2C_FLAG_WAITING_FLUSH      (1 << 1)
//  Repeat old data, no matter wether I2C_FLAG_WAITING_FLUSH is set.
#define I2C_FLAG_SEND_REPEAT        (1 << 2)
#define I2C_FLAG_FORCE_FLUSH        (1 << 3)

#define I2C_ERROR_LOCK_MUTEX_FAILED         -1
#define I2C_ERROR_FLAG_FLUSH_IS_SET         -2
#define I2C_ERROR_DATA_BLOCK_EMPTY          -3
#define I2C_NOTICE_LAST_DATA_SENT           1

#define I2C_SET_FLAG(target, f)             ((target)->flag |= (f))
#define I2C_CLEAR_FLAG(target, f)           ((target)->flag &= ~(f))
#define I2C_GET_FLAG(target)                ((target)->flag)
#define I2C_TEST_FLAG(target, mask)         ((target)->flag & mask)

typedef struct {
    char* pending_data;
    size_t pending_data_index;
    size_t pending_data_length;
    size_t pending_data_capcity;

    char* next_data;
    size_t next_data_length;
    size_t next_data_capcity;
    mutex next_data_mutex;

    uint32_t flag;
} I2C_SEND_T;

typedef struct {
    char* pending_data_buf;
    size_t pending_data_index;
    size_t pending_data_capcity;

    char* recv_data_buf;
    size_t recv_data_length;
    size_t recv_data_capcity;
    mutex recv_data_mutex;

    uint32_t flag;
} I2C_RECV_T;

void I2C_SEND_INIT(I2C_SEND_T* target, size_t buf_len);
void I2C_RECV_INIT(I2C_RECV_T* target, size_t buf_len);

void I2C_SEND_RESET(I2C_SEND_T* target);
void I2C_RECV_RESET(I2C_RECV_T* target);

// len is the length of data, return as actual length sended.
int I2C_Set_Send_Data(I2C_SEND_T* target, uint8_t* data, size_t* len);
// len is the capcisy of buf, return as the received data length.
int I2C_Get_Recv_Data(I2C_RECV_T* target, uint8_t* buf, size_t* len);

int I2C_Flush_Send_Data(I2C_SEND_T* target);
int I2C_Flush_Recv_Data(I2C_RECV_T* target);

// Only use these in I2C IRQn handler.
int I2C_Send_Byte(I2C_SEND_T* target, I2C_T* port, uint8_t* data);
int I2C_Recv_Byte(I2C_RECV_T* target, I2C_T* port, uint8_t* data);
#endif
