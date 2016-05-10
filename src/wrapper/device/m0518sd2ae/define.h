#ifndef __DEFINE_H__
#define __DEFINE_H__

#define PLLCON_SETTING              CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK                   50000000

#define PWM_PERIOD_US               11000
#define PWM_LOW_BOUND_US            1200
#define PWM_HI_BLUND_US             1800
#define PWM_PERIOD_LIMIT_US         11000
#define PWM_LOW_BOUND_LIMIT_US      1100
#define PWM_HI_BOUND_LIMIT_US       1900
#define PWM_GLOBAL_PRESCALE         5

#define Enable_PWM0_INT()       (PWM0->CAPIEN |= (0x15 << 8))
#define Disable_PWM0_INT()      (PWM0->CAPIEN &= (~(0x15 << 8)))
#define Enable_PWM1_INT()       (PWM1->CAPIEN |= (0x15 << 8))
#define Disable_PWM1_INT()      (PWM1->CAPIEN &= (~(0x15 << 8)))
#define Enable_I2C0_INT()       (I2C_EnableInt(I2C0))
#define Disable_I2C0_INT()      (I2C_DisableInt(I2C0))

#define PWM_GET_CMR(pwm, u32ChannelNum) ((pwm)->CMPBUF[(u32ChannelNum)])
#define PWM_GET_CNR(pwm, u32ChannelNum)  ((pwm)->PBUF[(((u32ChannelNum) >> 1) << 1)])
#define BPWM_GET_CMR(bpwm, u32ChannelNum) ((bpwm)->CMPBUF[(u32ChannelNum)])
#define BPWM_GET_CNR(bpwm) ((bpwm)->PBUF)

#define I2C_STATUS_TIMEOUT_VALUE                    0x00003fff
#define I2C_STATUS_CONTROL_EXPIRE_VALUE             0x0003ffff

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
