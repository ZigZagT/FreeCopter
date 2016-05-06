#ifndef __DEFINE_H__
#define __DEFINE_H__ 1

#define I2C_STATUS_BUS_ERROR                        0x00
#define I2C_STATUS_BUS_FREE                         0xf8

#define I2C_STATUS_MASTER_START                     0x08
#define I2C_STATUS_MASTER_RESTART                   0x10
#define I2C_STATUS_MASTER_SEND_ADDR_ACK             0x18
#define I2C_STATUS_MASTER_SEND_ADDR_NACK            0x20
#define I2C_STATUS_MASTER_SEND_DATA_ACK             0x28
#define I2C_STATUS_MASTER_SEND_DATA_NACK            0x30
#define I2C_STATUS_MASTER_ARBI_LOST                 0x38
#define I2C_STATUS_MASTER_RECV_ADDR_ACK             0x40
#define I2C_STATUS_MASTER_RECV_ADDR_NACK            0x48
#define I2C_STATUS_MASTER_RECV_DATA_ACK             0x50
#define I2C_STATUS_MASTER_RECV_DATA_NACK            0x58

#define I2C_STATUS_SLAVE_RESTART_STOP               0xa0
#define I2C_STATUS_SLAVE_SEND_ADDR_ACK              0xa8
#define I2C_STATUS_SLAVE_SEND_ARBI_LOST             0xb0
#define I2C_STATUS_SLAVE_SEND_DATA_ACK              0xb8
#define I2C_STATUS_SLAVE_SEND_DATA_NACK             0xc0
#define I2C_STATUS_SLAVE_SEND_LAST_DATA_ACK         0xc8
#define I2C_STATUS_SLAVE_RECV_ADDR_ACK              0x60
#define I2C_STATUS_SLAVE_RECV_ARBI_LOST             0x68
#define I2C_STATUS_SLAVE_RECV_DATA_ACK              0x80
#define I2C_STATUS_SLAVE_RECV_DATA_NACK             0x88

#define I2C_STATUS_GC_ADDR_ACK                      0x70
#define I2C_STATUS_GC_ARBI_LOST                     0x78
#define I2C_STATUS_GC_DATA_ACK                      0x90
#define I2C_STATUS_GC_DATA_NACK                     0x98

#endif